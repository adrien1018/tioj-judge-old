#include "utils.h"

#include <ftw.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <fstream>
#include <algorithm>

int IsDirectory(const std::string& path) {
  struct stat path_stat;
  IFERR(stat(path.c_str(), &path_stat))
    return -1;
  return S_ISDIR(path_stat.st_mode);
}

int IsRegularFile(const std::string& path) {
  struct stat path_stat;
  IFERR(stat(path.c_str(), &path_stat))
    return -1;
  return S_ISREG(path_stat.st_mode);
}

bool IsAbsolutePath(const std::string& path) {
  return path[0] == '/';
}

bool IsDownwardPath(const std::string& path) {
  return path != ".." &&
      path.find("/../") == std::string::npos &&
      (path.size() < 3 || (
          path.substr(0, 3) != "../" &&
          path.substr(path.size() - 3) != "/.."
      ));
}

// concatenate path1/path2; if path2 is absolute then return path2
std::string ConcatPath(const std::string& path1, const std::string& path2) {
  if (path2.empty()) return path1;
  if (path1.empty() || IsAbsolutePath(path2)) return path2;
  if (path1.back() == '/') return path1 + path2;
  return path1 + '/' + path2;
}

int mkdir_recursive(const char* path, mode_t mode) {
  if (!*path) {
    errno = ENOENT;
    return -1;
  }

  size_t length = strlen(path);
  char* path_copy = (char*) malloc(length + 1);
  if (!path_copy) {
    errno = ENOMEM;
    return -1;
  }
  strcpy(path_copy, path);

  char* ptr = path_copy;
  while (*++ptr) {
    if (*ptr == '/') {
      *ptr = '\0';
      IFERR(mkdir(path_copy, mode)) {
        if (errno != EEXIST) goto error;
      }
      *ptr = '/';
    }
  }
  if (*--ptr != '/') {
    IFERR(mkdir(path_copy, mode)) {
      if (errno != EEXIST) goto error;
    }
  }
  free(path_copy);
  return 0;

error:
  free(path_copy);
  return -1;
}

int UnlinkHelper(const char* fpath, const struct stat* sb, int typeflag,
                 struct FTW *ftwbuf) {
  return remove(fpath);
}

int rmdir_recursive(const char* path) {
  return nftw(path, UnlinkHelper, 128, FTW_DEPTH | FTW_PHYS);
}

int CopyFile(const std::string& source, const std::string& dest, mode_t perm) {
  pid_t pid = fork();
  IFERR(pid) return -1;
  if (pid == 0) {
    char buf[6];
    snprintf(buf, sizeof(buf), "0%o", (int)perm);
    execlp("install", "-D", "-m", buf, source.c_str(), dest.c_str(), nullptr);
  }
  int status;
  waitpid(pid, &status, 0);
  if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) return -1;
  return 0;
}

// split a comma-separated line into columns
std::vector<std::string> SplitLine(const std::string& a) {
  if (!a.size()) return std::vector<std::string>();
  bool flag1 = false, flag2 = false;
  std::vector<std::string> res(1);
  for (char i : a) {
    if (flag1) {
      if (i == '\"') flag1 = false, flag2 = true;
      else res.back().push_back(i);
    }
    else {
      if (i == '\"') {
        flag1 = true;
        if (flag2) flag2 = false, res.back().push_back('\"');
      }
      else if (i == ',') {
        res.push_back("");
        flag2 = false;
      }
      else res.back().push_back(i);
    }
  }
  return res;
}

// read a comma-separated csv file
StringTable ReadCsv(const std::string& filename) {
	std::fstream fin(filename, std::ios::in);
	if (!fin) return StringTable();

	std::string str, now;
	StringTable res;
	bool flag = false;
	while (getline(fin, str)) {
		now += str;
		flag ^= std::count(str.begin(), str.end(), '\"') & 1;
		if (!flag) { // total '\"' character count is even - line completed
			res.emplace_back(SplitLine(now));
			now.clear();
		}
		else now += '\n';
	}
	return res;
}

// combine columns into a single string
std::string FormatLine(std::vector<std::string>::const_iterator first,
                       std::vector<std::string>::const_iterator last) {
	std::string res, strtemp;
	size_t pos;
	for (; first != last; first++) {
		if (first->find('\"') != std::string::npos ||
        first->find(',') != std::string::npos ||
        first->find('\n') != std::string::npos) {
			res += '\"';
			strtemp = *first;
			pos = 0;
			while ((pos = strtemp.find('\"', pos)) != std::string::npos) {
			  strtemp.insert(pos, 1, '\"');
			  pos += 2;
			}
			res += strtemp;
			res += '\"';
		}
		else res += *first;
		res += ',';
	}
	res.pop_back();
	return res;
}

int WriteCsv(const StringTable& table, const std::string& filename)
{
	std::fstream fout(filename, std::ios::out | std::ios::binary);
	if (!fout) return -1;
	for (auto& i : table) fout << FormatLine(i.begin(), i.end()) << "\n";
	return 0;
}
