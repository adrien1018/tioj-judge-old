#include <unistd.h>
#include <sys/stat.h>

#include <chrono>
#include <random>
#include <thread>
#include <fstream>

#include <gtest/gtest.h>

#include "filestruct.h"
#include "utils.h"

const std::string kDataPath = ".";

void CreateRandomFile(const std::string& str) {
  static std::mt19937_64 gen;
  std::fstream fout(str, std::ios::out);
  for (int i = 0; i < 1000000; i++) {
    fout << std::string(std::uniform_int_distribution<int>(1, 11)(gen),
        std::uniform_int_distribution<char>(32, 125)(gen));
  }
}

int GetLinkNum(const std::string& str) {
  struct stat stats;
  if (stat(str.c_str(), &stats) < 0) ThrowErrno();
  return stats.st_nlink;
}

long long GetFileLength(const std::string& str) {
  struct stat stats;
  if (stat(str.c_str(), &stats) < 0) ThrowErrno();
  return stats.st_size;
}

extern const std::string kProblemBaseDir;
extern const std::string kCommonFilesDir;
extern const std::string kTestdataDir;
extern const std::string kProblemFilesDir;
extern const std::string kSubmissionsDir;
extern const std::string kStatusFilePath;

namespace {

class Environment : public ::testing::Environment {
 public:
  virtual void SetUp() {
    mkdir(kProblemBaseDir.c_str(), 0755);
  }
  virtual void TearDown() {
    RemoveRecursive(kProblemBaseDir);
  }
};

TEST(CreateProblemDirTest, Main) {
  CreateProblemDir(1);
  EXPECT_TRUE(IsDirectory(FSProblemDir(1)));
  EXPECT_TRUE(IsDirectory(ConcatPath(FSProblemDir(1), kProblemFilesDir)));
  EXPECT_TRUE(IsDirectory(ConcatPath(FSProblemDir(1), kTestdataDir)));
  EXPECT_TRUE(IsDirectory(ConcatPath(FSProblemDir(1), kCommonFilesDir)));
  EXPECT_TRUE(IsDirectory(ConcatPath(FSProblemDir(1), kSubmissionsDir)));
  EXPECT_TRUE(IsRegularFile(ConcatPath(FSProblemDir(1), kStatusFilePath)));
  RemoveRecursive(FSProblemDir(1));
}

class ProblemFileStructTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    CreateProblemDir(1);
    for (int i = 0; i < 20; i++) CreateRandomFile(FSProblemFilePath(1, i));
  }
  virtual void TearDown() {
    RemoveRecursive(FSProblemDir(1));
  }
};

TEST_F(ProblemFileStructTest, CompressAndDecompress) {
  std::atomic_bool intr(false);
  CompressProblemDir(1, intr);
  EXPECT_TRUE(IsRegularFile(FSProblemTarPath(1)));
  EXPECT_FALSE(FileExists(FSProblemDir(1)));
  EXPECT_TRUE(CheckProblemCompressed(1));
  std::cerr << "filesize = " << GetFileLength(FSProblemTarPath(1)) << '\n';
  DecompressProblemDir(1);
  EXPECT_TRUE(IsRegularFile(FSProblemFilePath(1, 14)));
  EXPECT_TRUE(IsRegularFile(FSProblemFilePath(1, 0)));
  EXPECT_FALSE(FileExists(FSProblemTarPath(1)));
  EXPECT_TRUE(IsDirectory(FSProblemDir(1)));
  EXPECT_FALSE(CheckProblemCompressed(1));
}

void IntrThread(std::atomic_bool* s) {
  sleep(1);
  *s = true;
}

TEST_F(ProblemFileStructTest, CompressInterrupt) {
  std::atomic_bool* intr = new std::atomic_bool(true);
  CompressProblemDir(1, *intr);
  EXPECT_FALSE(FileExists(FSProblemTarPath(1)));
  EXPECT_TRUE(IsDirectory(FSProblemDir(1)));
  EXPECT_FALSE(CheckProblemCompressed(1));
  *intr = false;
  std::thread thr(IntrThread, intr);
  CompressProblemDir(1, *intr);
  thr.join();
  EXPECT_FALSE(FileExists(FSProblemTarPath(1)));
  EXPECT_TRUE(IsDirectory(FSProblemDir(1)));
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

  EXPECT_EQ(3, GetLinkNum(FSTestdataFilePath(1, 5, 0)));
  EXPECT_EQ(3, GetLinkNum(FSTestdataFilePath(1, 2, 0)));
  EXPECT_EQ(2, GetLinkNum(FSTestdataFilePath(1, 3, 1)));
  EXPECT_EQ(3, GetLinkNum(FSProblemFilePath(1, 3)));
  EXPECT_EQ(2, GetLinkNum(FSProblemFilePath(1, 13)));
  EXPECT_EQ(2, GetLinkNum(FSCommonFilePath(1, 0)));
  EXPECT_EQ(2, GetLinkNum(FSCommonFilePath(1, 3)));
  EXPECT_FALSE(FileExists(FSTestdataFilePath(1, 7, 0)));
  EXPECT_FALSE(FileExists(FSTestdataFilePath(1, 0, 3)));
  EXPECT_FALSE(FileExists(FSCommonFilePath(1, 4)));

  std::atomic_bool intr(false);
  CompressProblemDir(1, intr);
  std::cerr << "filesize = " << GetFileLength(FSProblemTarPath(1)) << '\n';
  DecompressProblemDir(1);
}

TEST_F(ProblemFileStructTest, Empty) {
  EXPECT_TRUE(1);
}

} // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new Environment);
  return RUN_ALL_TESTS();
}
