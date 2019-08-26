#include "filestruct.h"

#include "config.h"
#include "utils.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>

#define IFERR_THROW(x) if ((x) < 0) ThrowErrno()

namespace fs = std::filesystem;

// declare extern for testing program to link
extern const fs::path kProblemBaseDir;
extern const fs::path kCommonFilesDir;
extern const fs::path kTestdataDir;
extern const fs::path kProblemFilesDir;
extern const fs::path kSubmissionsDir;

const fs::path kProblemBaseDir = kDataPath/"problem";
const fs::path kCommonFilesDir = "commonfiles";
const fs::path kTestdataDir = "testdata";
const fs::path kProblemFilesDir = "problemfiles";
const fs::path kSubmissionsDir = "submissions";

inline std::string ProbDirName(int id) {
  return FormatStr("%06d", id);
}

fs::path FSProblemDir(int id) {
  return kProblemBaseDir/ProbDirName(id);
}
fs::path FSSubmissionDir(int prob_id, int sub_id) {
  return FSProblemDir(prob_id)/kSubmissionsDir/FormatStr("%08d", sub_id);
}
fs::path FSSubmissionFilePath(int prob_id, int sub_id, int file_id) {
  return FSSubmissionDir(prob_id, sub_id)/FormatStr("%d", file_id);
};
fs::path FSProblemTarPath(int id) {
  fs::path ret = FSProblemDir(id);
  ret += ".tar.gz";
  return ret;
}
fs::path FSTestdataFilePath(int prob_id, int testdata_id, int file_id) {
  return FSProblemDir(prob_id)/kTestdataDir/
      FormatStr("%04d_%02d", testdata_id, file_id);
}
fs::path FSProblemFilePath(int prob_id, int file_id) {
  return FSProblemDir(prob_id)/kProblemFilesDir/FormatStr("%04d", file_id);
}
fs::path FSCommonFilePath(int prob_id, int file_id) {
  return FSProblemDir(prob_id)/kCommonFilesDir/FormatStr("%03d", file_id);
}

void CreateProblemDir(int id) {
  const fs::path base = FSProblemDir(id);
  fs::create_directory(base);
  fs::create_directory(base/kProblemFilesDir);
  fs::create_directory(base/kSubmissionsDir);
  fs::create_directory(base/kTestdataDir);
  fs::create_directory(base/kCommonFilesDir);
}

void CreateSubmissionDir(int problem_id, int submission_id) {
  fs::create_directory(FSSubmissionDir(problem_id, submission_id));
}

bool CheckProblemCompressed(int id) {
  return !fs::exists(FSProblemDir(id)) && fs::exists(FSProblemTarPath(id));
}

void CompressProblemDir(int id, volatile std::atomic_bool& interrupt) {
  const fs::path base = FSProblemDir(id);
  if (!fs::exists(base)) {
    throw std::invalid_argument("Problem dir does not exist");
  }
  const fs::path tarpath = FSProblemTarPath(id);
  pid_t pid = fork();
  IFERR_THROW(pid);
  if (pid == 0) {
    // tar can deal with hardlinks
    execlp("tar", "tar", "-zcf", tarpath.c_str(), "-C", kProblemBaseDir.c_str(),
           ProbDirName(id).c_str(), nullptr);
    // exec not success
    exit(1);
  }
  // parent
  int status = 0;
  bool intr_flag = false;
  while (true) { // polling
    usleep(300000);
    if (interrupt) {
      // note that pid might be a zombie now, but kill still works
      IFERR_THROW(kill(pid, SIGINT));
      waitpid(pid, &status, 0);
      intr_flag = true;
      break;
    }
    int ret = waitpid(pid, &status, WNOHANG);
    IFERR_THROW(ret);
    if (ret != 0) break; // exited
  }
  if (!WIFEXITED(status) || WEXITSTATUS(status) != 0 || intr_flag) {
    // interrupted / not exited normally
    fs::remove(tarpath);
    if (!intr_flag) throw std::runtime_error("tar exited with nonzero status");
  } else {
    // successfully compressed and not interrupted, remove base dir
    fs::remove_all(base);
  }
}

void DecompressProblemDir(int id) {
  const fs::path tarpath = FSProblemTarPath(id);
  if (!fs::exists(tarpath)) {
    throw std::invalid_argument("Problem tarball does not exist");
  }
  pid_t pid = fork();
  IFERR_THROW(pid);
  if (pid == 0) {
    execlp("tar", "tar", "-zxf", tarpath.c_str(), "-C", kProblemBaseDir.c_str(),
           nullptr);
    // exec not success
    exit(1);
  }
  // parent
  int status = 0;
  waitpid(pid, &status, 0);
  if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
    throw std::runtime_error("Problem dir decompression failed");
  } else {
    fs::remove(tarpath);
  }
}

void UpdateFileLinks(const ProblemSettings& ps) {
  const fs::path testdatadir = FSProblemDir(ps.problem_id)/kTestdataDir;
  const fs::path commonfilesdir = FSProblemDir(ps.problem_id)/kCommonFilesDir;
  fs::remove_all(testdatadir);
  fs::remove_all(commonfilesdir);
  fs::create_directory(testdatadir);
  fs::create_directory(commonfilesdir);
  for (size_t i = 0; i < ps.common_files.size(); i++) {
    fs::create_hard_link(
        FSProblemFilePath(ps.problem_id, ps.common_files[i].file_id),
        FSCommonFilePath(ps.problem_id, i));
  }
  for (size_t i = 0; i < ps.testdata.size(); i++) {
    for (size_t j = 0; j < ps.testdata[i].file_id.size(); j++) {
      fs::create_hard_link(
          FSProblemFilePath(ps.problem_id, ps.testdata[i].file_id[j]),
          FSTestdataFilePath(ps.problem_id, i, j));
    }
  }
}

void DeleteProblemFiles(int problem_id) {
  if (CheckProblemCompressed(problem_id)) {
    fs::remove_all(FSProblemTarPath(problem_id));
  } else {
    fs::remove_all(FSProblemDir(problem_id));
  }
}

void DeleteSubmissionFiles(int problem_id, int submission_id) {
  fs::remove_all(FSSubmissionDir(problem_id, submission_id));
}
