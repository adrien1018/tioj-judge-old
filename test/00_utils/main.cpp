#include <fcntl.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdio>
#include <set>
#include <vector>
#include <gtest/gtest.h>

#include "utils.h"

namespace {

class Environment : public ::testing::Environment {
 public:
  virtual void SetUp() {
    creat("test_regular", 0644);
    mkdir("test_directory", 0755);
    mkfifo("test_fifo", 0644);
    symlink("test_regular", "test_symlink");
    symlink("test_notexist", "test_dangling_symlink");
    struct sockaddr_un namesock;
    namesock.sun_family = AF_UNIX;
    strcpy(namesock.sun_path, "test_socket");
    int fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    bind(fd, (struct sockaddr*)&namesock, sizeof(struct sockaddr_un));
    close(fd);
  }
  virtual void TearDown() {
    unlink("test_regular");
    unlink("test_fifo");
    unlink("test_symlink");
    unlink("test_dangling_symlink");
    unlink("test_socket");
    rmdir("test_directory");
  }
};

TEST(FileExistsTest, Main) {
  EXPECT_TRUE(FileExists("test_regular"));
  EXPECT_TRUE(FileExists("test_directory"));
  EXPECT_TRUE(FileExists("test_symlink"));
  EXPECT_TRUE(FileExists("test_fifo"));
  EXPECT_TRUE(FileExists("test_socket"));
  EXPECT_TRUE(FileExists("/dev/zero"));
  EXPECT_FALSE(FileExists("test_dangling_symlink"));
  EXPECT_FALSE(FileExists("test_notexist"));
  EXPECT_FALSE(FileExists(""));
}

TEST(RegularFileTest, Main) {
  EXPECT_TRUE(IsRegularFile("test_regular"));
  EXPECT_FALSE(IsRegularFile("test_directory"));
  EXPECT_FALSE(IsRegularFile("test_symlink"));
  EXPECT_FALSE(IsRegularFile("test_fifo"));
  EXPECT_FALSE(IsRegularFile("test_socket"));
  EXPECT_FALSE(IsRegularFile("/dev/zero"));
  EXPECT_FALSE(IsRegularFile("test_dangling_symlink"));
  EXPECT_THROW(IsRegularFile("test_notexist"), std::system_error);
}

TEST(DirectoryTest, Main) {
  EXPECT_FALSE(IsDirectory("test_regular"));
  EXPECT_TRUE(IsDirectory("test_directory"));
  EXPECT_FALSE(IsDirectory("test_symlink"));
  EXPECT_FALSE(IsDirectory("test_fifo"));
  EXPECT_FALSE(IsDirectory("test_socket"));
  EXPECT_FALSE(IsDirectory("/dev/zero"));
  EXPECT_FALSE(IsDirectory("test_dangling_symlink"));
  EXPECT_THROW(IsDirectory("test_notexist"), std::system_error);
}

TEST(ValidFilenameTest, Length) {
  EXPECT_FALSE(IsValidFilename(""));
  EXPECT_TRUE(IsValidFilename("a"));
  EXPECT_TRUE(IsValidFilename(std::string(255, 'a')));
  EXPECT_FALSE(IsValidFilename(std::string(256, 'a')));
}

TEST(ValidFilenameTest, Charset) {
  std::string str;
  for (int i = 1; i < 256; i++) {
    if (i != (int)'/') str.push_back(i);
  }
  EXPECT_TRUE(IsValidFilename(str));
  str[2] = 0;
  EXPECT_FALSE(IsValidFilename(str));
  str[2] = '/';
  EXPECT_FALSE(IsValidFilename(str));
  str[2] = '\\';
  EXPECT_TRUE(IsValidFilename(str));
}

TEST(AbsolutePathTest, Main) {
  EXPECT_TRUE(IsAbsolutePath("/bin"));
  EXPECT_TRUE(IsAbsolutePath("/"));
  EXPECT_TRUE(IsAbsolutePath("/bin/"));
  EXPECT_FALSE(IsAbsolutePath("bin/"));
  EXPECT_FALSE(IsAbsolutePath("bin"));
}

TEST(DownwardPathTest, Main) {
  EXPECT_TRUE(IsDownwardPath("/usr/lib/"));
  EXPECT_TRUE(IsDownwardPath("/"));
  EXPECT_TRUE(IsDownwardPath("/usr/lib/libsvm.so"));
  EXPECT_TRUE(IsDownwardPath("usr/lib/libsvm.so"));
  EXPECT_TRUE(IsDownwardPath("usr/lib/"));
  EXPECT_TRUE(IsDownwardPath("usr/lib.././l.."));
  EXPECT_TRUE(IsDownwardPath("..usr/..lib/./."));
  EXPECT_TRUE(IsDownwardPath("usr/lib/./."));
  EXPECT_TRUE(IsDownwardPath("/./usr/lib/./."));
  EXPECT_FALSE(IsDownwardPath("/./usr/lib/./.."));
  EXPECT_FALSE(IsDownwardPath("/./usr/lib/../"));
  EXPECT_FALSE(IsDownwardPath("/./usr/lib/../lib"));
  EXPECT_FALSE(IsDownwardPath(".."));
  EXPECT_FALSE(IsDownwardPath("../usr/lib/"));
  EXPECT_FALSE(IsDownwardPath("../usr/lib"));
}

TEST(ConcatPathTest, Empty) {
  EXPECT_EQ("/test", ConcatPath("/test", ""));
  EXPECT_EQ("/test", ConcatPath("", "/test"));
  EXPECT_EQ("test", ConcatPath("", "test"));
  EXPECT_EQ("test", ConcatPath("test", ""));
}

TEST(ConcatPathTest, SecondAbsolute) {
  EXPECT_EQ("/test", ConcatPath("/test/1/2/3/4", "/test"));
  EXPECT_EQ("/1/2/3/4", ConcatPath("test", "/1/2/3/4"));
}

TEST(ConcatPathTest, Main) {
  EXPECT_EQ("/test", ConcatPath("/", "test"));
  EXPECT_EQ("/1/2/3/4", ConcatPath("/1/2", "3/4"));
  EXPECT_EQ("/test/testp", ConcatPath("/test/", "testp"));
  EXPECT_EQ("/test/testp/", ConcatPath("/test/", "testp/"));
}

TEST(GetFilenameTest, Main) {
  EXPECT_EQ("1", GetFilename("1"));
  EXPECT_EQ("", GetFilename("/"));
  EXPECT_EQ("d", GetFilename("/a/b/c/d"));
}

TEST(RealPathTest, Main) {
  char* ptr;
  ptr = getcwd(nullptr, 0);
  std::string now(ptr);
  free(ptr);
  EXPECT_EQ(ConcatPath(now, "test_regular"), RealPath("test_symlink"));
  EXPECT_EQ(ConcatPath(now, "test_regular"), RealPath("test_regular"));
  EXPECT_EQ(ConcatPath(now, "test_fifo"), RealPath("test_fifo"));
  EXPECT_THROW(RealPath("test_dangling_symlink"), std::system_error);
  EXPECT_THROW(RealPath("test_notexist"), std::system_error);
}

TEST(RemoveRecursiveTest, Main) {
  mkdir("test1", 0755);
  mkdir("test1/1", 0755);
  mkdir("test1/2", 0755);
  mkdir("test1/3", 0755);
  mkdir("test1/2/1", 0755);
  mkdir("test1/2/2", 0755);
  mkdir("test1/2/qwercq qwer", 0755);
  mkdir("test1/2/werv **ercw.,.", 0755);
  mkdir("test1/2/werv **ercw.,./wervt", 0755);
  creat("test1/1.1", 0644);
  creat("test1/2/1.1", 0644);
  creat("test1/2/1.2", 0644);
  creat("test1/2/1.3", 0644);
  creat("test1/2/1/1.1", 0644);
  creat("test_rm", 0644);
  EXPECT_NO_THROW(RemoveRecursive("test1/2/2"));
  EXPECT_NO_THROW(RemoveRecursive("test1"));
  EXPECT_NO_THROW(RemoveRecursive("test_rm"));
  EXPECT_FALSE(FileExists("test1"));
  EXPECT_FALSE(FileExists("test_rm"));
  EXPECT_THROW(RemoveRecursive("test_rm"), std::system_error);
}

TEST(FormatStrTest, Main) {
  EXPECT_EQ("000123    12", FormatStr("%06d%5s%s", 123, std::string("1"), "2"));
}

TEST(ValidDBNameTest, Length) {
  EXPECT_FALSE(IsValidDBName(""));
  EXPECT_TRUE(IsValidDBName("a"));
  EXPECT_TRUE(IsValidDBName(std::string(64, 'a')));
  EXPECT_FALSE(IsValidDBName(std::string(65, 'a')));
}

TEST(ValidDBNameTest, Charset) {
  EXPECT_FALSE(IsValidDBName("1a"));
  EXPECT_FALSE(IsValidDBName("a-a"));
  EXPECT_FALSE(IsValidDBName("a&a"));
  EXPECT_FALSE(IsValidDBName("a)a"));
  EXPECT_FALSE(IsValidDBName("a).a"));
  EXPECT_FALSE(IsValidDBName("喵"));
  EXPECT_TRUE(IsValidDBName("____"));
  EXPECT_TRUE(IsValidDBName("abcdefghijklmnopqrstuvwxyz_0123456789"));
  EXPECT_TRUE(IsValidDBName("ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789"));
}

TEST(DateTimeStrTest, Main) {
  EXPECT_EQ("1970-01-01 00:00:00.000", DateTimeStr(62167219200000));
  EXPECT_EQ("1969-12-31 23:59:59.999", DateTimeStr(62167219199999));
  EXPECT_EQ("2018-06-12 06:15:59.013", DateTimeStr(63696003359013));
  EXPECT_EQ("2018-10-26 06:15:05.003", DateTimeStr(63707753705003));
  EXPECT_EQ("1718-06-12 06:15:59.213", DateTimeStr(54228896159213));
  EXPECT_EQ("2388-03-01 08:03:01.831", DateTimeStr(75363206581831));
}

TEST(DateTimeValTest, Main) {
  EXPECT_EQ(DateTimeVal("1970-01-01 00:00:00.000"), 62167219200000);
  EXPECT_EQ(DateTimeVal("1969-12-31 23:59:59.999"), 62167219199999);
  EXPECT_EQ(DateTimeVal("2018-06-12 06:15:59.013"), 63696003359013);
  EXPECT_EQ(DateTimeVal("2018-10-26 06:15:05.003"), 63707753705003);
  EXPECT_EQ(DateTimeVal("1718-06-12 06:15:59.213"), 54228896159213);
  EXPECT_EQ(DateTimeVal("2388-03-01 08:03:01.831"), 75363206581831);
  EXPECT_THROW(DateTimeVal("2388-03-01 08:03:01.8311"), std::invalid_argument);
  EXPECT_THROW(DateTimeVal("2388-3-01 08:03:01.811"), std::invalid_argument);
  EXPECT_THROW(DateTimeVal("388-03-01 08:03:01.811"), std::invalid_argument);
}

typedef std::vector<std::string> StrArray;

TEST(SplitStringTest, Basic) {
  EXPECT_EQ(StrArray({"1", "2", "3", "4", "5"}), SplitString("1,2,3,4,5"));
  EXPECT_EQ(StrArray({"123"}), SplitString("123"));
  EXPECT_EQ(StrArray(), SplitString(""));
}

TEST(SplitStringTest, SpecialChar) {
  EXPECT_EQ(StrArray({"1", "2\"", "2a"}), SplitString("1,\"2\"\"\",2a"));
  EXPECT_EQ(StrArray({"1", "2\n", "2a,"}), SplitString("1,\"2\n\",\"2a,\""));
  EXPECT_EQ(StrArray({"喔", "哈"}), SplitString("喔,哈"));
}

TEST(MergeStringTest, String) {
  StrArray vec;
  vec = StrArray({"1", "2", "3", "4", "5"});
  EXPECT_EQ("1,2,3,4,5", MergeString(vec.begin(), vec.end()));
  EXPECT_EQ("1,2,3,4,5", MergeString(vec));
  vec.clear();
  EXPECT_EQ("", MergeString(vec));
  EXPECT_EQ("\"\n\",\"123\"\"123\",\",,,\"",
      MergeString(StrArray({"\n", "123\"123", ",,,"})));
}

TEST(MergeStringTest, Func) {
  std::set<int> a({3,7,4,5,2,6,1,7});
  auto IntToStr = [](int a){return std::to_string(a);};
  EXPECT_EQ("1,2,3,4,5,6,7", MergeString(a.begin(), a.end(), IntToStr));
  EXPECT_EQ("1,2,3,4,5,6,7", MergeString(a, IntToStr));
}

} // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new Environment);
  return RUN_ALL_TESTS();
}
