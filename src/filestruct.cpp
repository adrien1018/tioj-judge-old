#include "filestruct.h"

#include "config.h"
#include "utils.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>

#define IFERR_THROW(x) if ((x) < 0) ThrowErrno()

// declare extern for testing program to link
extern const std::string kProblemBaseDir;
extern const std::string kCommonFilesDir;
extern const std::string kTestdataDir;
extern const std::string kProblemFilesDir;
extern const std::string kSubmissionsDir;
extern const std::string kStatusFilePath;

const std::string kProblemBaseDir = ConcatPath(kDataPath, "problem");
const std::string kCommonFilesDir = "commonfiles";
const std::string kTestdataDir = "testdata";
const std::string kProblemFilesDir = ".problemfiles";
const std::string kSubmissionsDir = "submissions";
const std::string kStatusFilePath = "status";

inline std::string ProbDirName(int id) {
  return FormatStr("%05d", id);
}

std::string FSProblemDir(int id) {
  return ConcatPath(kProblemBaseDir, ProbDirName(id));
}
std::string FSProblemTarPath(int id) {
  return FSProblemDir(id) + ".tar.gz";
}
std::string FSTestdataFilePath(int prob_id, int testdata_id, int file_id) {
  return ConcatPath(ConcatPath(FSProblemDir(prob_id), kTestdataDir),
                    FormatStr("%04d_%02d", testdata_id, file_id));
}
std::string FSProblemFilePath(int prob_id, int file_id) {
  return ConcatPath(ConcatPath(FSProblemDir(prob_id), kProblemFilesDir),
                    FormatStr("%05d", file_id));
}
std::string FSCommonFilePath(int prob_id, int file_id) {
  return ConcatPath(ConcatPath(FSProblemDir(prob_id), kCommonFilesDir),
                    FormatStr("%03d", file_id));
}

void CreateProblemDir(int id) {
  const std::string base = FSProblemDir(id) + "/";
  IFERR_THROW(mkdir(base.c_str(), 0755));
  IFERR_THROW(mkdir((base + kProblemFilesDir).c_str(), 0755));
  IFERR_THROW(mkdir((base + kSubmissionsDir).c_str(), 0755));
  IFERR_THROW(mkdir((base + kTestdataDir).c_str(), 0755));
  IFERR_THROW(mkdir((base + kCommonFilesDir).c_str(), 0755));
  IFERR_THROW(creat((base + kStatusFilePath).c_str(), 0644));
}

bool CheckProblemCompressed(int id) {
  return !FileExists(FSProblemDir(id));
}

void CompressProblemDir(int id, volatile std::atomic_bool& interrupt) {
  const std::string base = FSProblemDir(id);
  if (!FileExists(base)) {
    throw std::invalid_argument("Problem dir does not exist");
  }
  const std::string tarpath = FSProblemTarPath(id);
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
    unlink(tarpath.c_str());
    if (!intr_flag) throw std::runtime_error("tar exited with nonzero status");
  } else {
    // successfully compressed and not interrupted, remove base dir
    RemoveRecursive(base);
  }
}

void DecompressProblemDir(int id) {
  std::string tarpath = FSProblemTarPath(id);
  if (!FileExists(tarpath)) {
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
    unlink(tarpath.c_str());
  }
}

void UpdateFileLinks(const ProblemSettings& ps) {
  auto Link = [](const std::string& a, const std::string& b) {
    IFERR_THROW(link(a.c_str(), b.c_str()));
  };

  const std::string testdatadir = ConcatPath(
      FSProblemDir(ps.problem_id), kTestdataDir);
  const std::string commonfilesdir = ConcatPath(
      FSProblemDir(ps.problem_id), kCommonFilesDir);
  RemoveRecursive(testdatadir);
  RemoveRecursive(commonfilesdir);
  IFERR_THROW(mkdir(testdatadir.c_str(), 0755));
  IFERR_THROW(mkdir(commonfilesdir.c_str(), 0755));
  for (size_t i = 0; i < ps.common_files.size(); i++) {
    Link(FSProblemFilePath(ps.problem_id, ps.common_files[i].file_id),
         FSCommonFilePath(ps.problem_id, i));
  }
  for (size_t i = 0; i < ps.testdata.size(); i++) {
    for (size_t j = 0; j < ps.testdata[i].file_id.size(); j++) {
      Link(FSProblemFilePath(ps.problem_id, ps.testdata[i].file_id[j]),
           FSTestdataFilePath(ps.problem_id, i, j));
    }
  }
}
