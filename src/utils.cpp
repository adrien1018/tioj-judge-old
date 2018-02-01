#include "utils.h"

#include <ftw.h>

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

std::string ConcatPath(const std::string& path1, const std::string& path2) {
  if (path2.empty()) return path1;
  if (path1.empty() || path2[0] == '/') return path2;
  if (path1.back() == '/') return path1 + path2;
  return path1 + '/' + path2;
}

int mkdir_recursive(const char* path, mode_t mode) {
  size_t length = strlen(path);
  char* path_copy = (char*) malloc(length + 1);
  strcpy(path_copy, path);

  char* ptr = path_copy;
  for (; *ptr; ptr++) {
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

int UnlinkHelper(const char* fpath, const struct stat* sb, int typeflag) {
  return remove(fpath);
}

int rmdir_recursive(const char* path) {
  return ftw(path, UnlinkHelper, 128);
}
