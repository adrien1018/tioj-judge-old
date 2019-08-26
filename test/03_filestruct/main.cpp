#include <unistd.h>
#include <sys/stat.h>

#include <chrono>
#include <random>
#include <thread>
#include <fstream>

#include <gtest/gtest.h>

#include "filestruct.h"
#include "utils.h"

namespace fs = std::filesystem;

const fs::path kDataPath = ".";

void CreateRandomFile(const std::string& str) {
  static std::mt19937_64 gen;
  std::fstream fout(str, std::ios::out);
  for (int i = 0; i < 500000; i++) {
    fout << std::string(std::uniform_int_distribution<int>(1, 11)(gen),
        std::uniform_int_distribution<char>(32, 125)(gen));
  }
}

extern const std::string kProblemBaseDir;
extern const std::string kCommonFilesDir;
extern const std::string kTestdataDir;
extern const std::string kProblemFilesDir;
extern const std::string kSubmissionsDir;

namespace {

class Environment : public ::testing::Environment {
 public:
  virtual void SetUp() {
    mkdir(kProblemBaseDir.c_str(), 0755);
  }
  virtual void TearDown() {
    fs::remove_all(kProblemBaseDir);
  }
};

TEST(PathTest, Main) {
  EXPECT_EQ(fs::relative(FSProblemDir(23)), "problem/000023");
  EXPECT_EQ(fs::relative(FSSubmissionDir(23, 34)), "problem/000023/submissions/00000034");
  EXPECT_EQ(fs::relative(FSSubmissionFilePath(23, 34, 45)), "problem/000023/submissions/00000034/45");
  EXPECT_EQ(fs::relative(FSProblemTarPath(23)), "problem/000023.tar.gz");
  EXPECT_EQ(fs::relative(FSTestdataFilePath(23, 34, 1)), "problem/000023/testdata/0034_01");
  EXPECT_EQ(fs::relative(FSProblemFilePath(23, 2)), "problem/000023/problemfiles/0002");
  EXPECT_EQ(fs::relative(FSCommonFilePath(23, 2)), "problem/000023/commonfiles/002");
}

TEST(ProblemDirTest, Main) {
  CreateProblemDir(1);
  EXPECT_TRUE(fs::is_directory(FSProblemDir(1)));
  EXPECT_TRUE(fs::is_directory(FSProblemDir(1)/kProblemFilesDir));
  EXPECT_TRUE(fs::is_directory(FSProblemDir(1)/kTestdataDir));
  EXPECT_TRUE(fs::is_directory(FSProblemDir(1)/kCommonFilesDir));
  EXPECT_TRUE(fs::is_directory(FSProblemDir(1)/kSubmissionsDir));
  CreateSubmissionDir(1, 10);
  EXPECT_TRUE(fs::is_directory(FSSubmissionDir(1, 10)));
  DeleteProblemFiles(1);
  EXPECT_FALSE(fs::exists(FSProblemDir(1)));
}

TEST(ProblemDirTest, DeleteCompress) {
  std::atomic_bool intr(false);
  CreateProblemDir(1);
  CompressProblemDir(1, intr);
  DeleteProblemFiles(1);
  EXPECT_FALSE(fs::exists(FSProblemDir(1)));
  EXPECT_FALSE(fs::exists(FSProblemTarPath(1)));
  EXPECT_FALSE(CheckProblemCompressed(1));
}

class ProblemFileStructTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    CreateProblemDir(1);
    for (int i = 0; i < 20; i++) CreateRandomFile(FSProblemFilePath(1, i));
    CreateSubmissionDir(1, 10);
    CreateSubmissionDir(1, 11);
    for (int i = 0; i < 3; i++) CreateRandomFile(FSSubmissionFilePath(1, 10, i));
    for (int i = 0; i < 3; i++) CreateRandomFile(FSSubmissionFilePath(1, 11, i));
  }
  virtual void TearDown() {
    DeleteProblemFiles(1);
  }
};

TEST_F(ProblemFileStructTest, CompressAndDecompress) {
  std::atomic_bool intr(false);
  CompressProblemDir(1, intr);
  EXPECT_TRUE(fs::is_regular_file(FSProblemTarPath(1)));
  EXPECT_FALSE(fs::exists(FSProblemDir(1)));
  EXPECT_TRUE(CheckProblemCompressed(1));
  std::cerr << "filesize = " << fs::file_size(FSProblemTarPath(1)) << '\n';
  DecompressProblemDir(1);
  EXPECT_TRUE(fs::is_regular_file(FSProblemFilePath(1, 14)));
  EXPECT_TRUE(fs::is_regular_file(FSProblemFilePath(1, 0)));
  EXPECT_FALSE(fs::exists(FSProblemTarPath(1)));
  EXPECT_TRUE(fs::is_directory(FSProblemDir(1)));
  EXPECT_FALSE(CheckProblemCompressed(1));
}

void IntrThread(std::atomic_bool* s) {
  sleep(1);
  *s = true;
}

TEST_F(ProblemFileStructTest, CompressInterrupt) {
  std::atomic_bool* intr = new std::atomic_bool(true);
  CompressProblemDir(1, *intr);
  EXPECT_FALSE(fs::exists(FSProblemTarPath(1)));
  EXPECT_TRUE(fs::is_directory(FSProblemDir(1)));
  EXPECT_FALSE(CheckProblemCompressed(1));
  *intr = false;
  std::thread thr(IntrThread, intr);
  CompressProblemDir(1, *intr);
  thr.join();
  EXPECT_FALSE(fs::exists(FSProblemTarPath(1)));
  EXPECT_TRUE(fs::is_directory(FSProblemDir(1)));
  EXPECT_FALSE(CheckProblemCompressed(1));
  delete intr;
}

TEST_F(ProblemFileStructTest, FileLink) {
  ProblemSettings ps;
  ps.problem_id = 1;
  ps.file_per_testdata = 2;
  ps.execution_type = ProblemSettings::kExecMultiPhaseSeparated;
  ps.execution_times = 4;
  ProblemSettings::Testdata ts;
  ts.file_id = {0, 5};
  ps.testdata.push_back(ts);
  ts.file_id = {1, 7};
  ps.testdata.push_back(ts);
  ts.file_id = {3, 2};
  ps.testdata.push_back(ts);
  ts.file_id = {11, 8};
  ps.testdata.push_back(ts);
  ts.file_id = {4, 9};
  ps.testdata.push_back(ts);
  ts.file_id = {3, 6};
  ps.testdata.push_back(ts);
  ts.file_id = {10, 12};
  ps.testdata.push_back(ts);
  ProblemSettings::CommonFile cf;
  cf.usage = ProblemSettings::CommonFile::kLib;
  cf.lib_lang = kLangC;
  cf.path = "lib1.h";
  cf.file_id = 13; cf.id = 0;
  ps.common_files.push_back(cf);
  cf.file_id = 14; cf.id = 1;
  ps.common_files.push_back(cf);
  cf.file_id = 15; cf.id = 2;
  ps.common_files.push_back(cf);
  cf.file_id = 16; cf.id = 3;
  ps.common_files.push_back(cf);

  using namespace std::chrono;
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  UpdateFileLinks(ps);
  int ms = duration_cast<duration<double>>(high_resolution_clock::now() - t1).count() * 1000;
  std::cerr << "time = " << ms << " ms\n";

  EXPECT_EQ(3, fs::hard_link_count(FSTestdataFilePath(1, 5, 0)));
  EXPECT_EQ(3, fs::hard_link_count(FSTestdataFilePath(1, 2, 0)));
  EXPECT_EQ(2, fs::hard_link_count(FSTestdataFilePath(1, 3, 1)));
  EXPECT_EQ(3, fs::hard_link_count(FSProblemFilePath(1, 3)));
  EXPECT_EQ(2, fs::hard_link_count(FSProblemFilePath(1, 13)));
  EXPECT_EQ(2, fs::hard_link_count(FSCommonFilePath(1, 0)));
  EXPECT_EQ(2, fs::hard_link_count(FSCommonFilePath(1, 3)));
  EXPECT_FALSE(fs::exists(FSTestdataFilePath(1, 7, 0)));
  EXPECT_FALSE(fs::exists(FSTestdataFilePath(1, 0, 3)));
  EXPECT_FALSE(fs::exists(FSCommonFilePath(1, 4)));

  std::atomic_bool intr(false);
  CompressProblemDir(1, intr);
  std::cerr << "filesize = " << fs::file_size(FSProblemTarPath(1)) << '\n';
  DecompressProblemDir(1);
}

TEST_F(ProblemFileStructTest, Empty) {
  EXPECT_TRUE(1);
}

TEST_F(ProblemFileStructTest, Submission) {
  EXPECT_TRUE(fs::is_directory(FSSubmissionDir(1, 10)));
  EXPECT_TRUE(fs::is_directory(FSSubmissionDir(1, 11)));
  EXPECT_TRUE(fs::is_regular_file(FSSubmissionFilePath(1, 10, 0)));
  DeleteSubmissionFiles(1, 10);
  EXPECT_FALSE(fs::exists(FSSubmissionDir(1, 10)));
}

} // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new Environment);
  return RUN_ALL_TESTS();
}
