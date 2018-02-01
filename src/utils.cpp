#include "utils.h"

int IsDirectory(const std::string& path) {
  struct stat path_stat;
  IFERR(stat(path, &path_stat))
    return -1;
  return S_ISDIR(path_stat.st_mode);
}

int IsRegularFile(const std::string& path) {
  struct stat path_stat;
  IFERR(stat(path, &path_stat))
    return -1;
  return S_ISREG(path_stat.st_mode);
}

bool IsAbsolutePath(const std::string& path) {
  return path[0] == '/';
}

bool IsDownwardPath(const std::string& path) {
  return path != ".." &&
      path.find("/../") == string::npos &&
      (path.size() < 3 || (
          path.substr(0, 3) != "../" &&
          path.substr(path.size() - 3) != "/.."
      ));
}

int mkdir_recursive(const std::string& path, mode_t mode) {
  bool slash_end = path.back() == '/';
  for (size_t position = 0; position != std::string::npos;
       position = path.find('/', position + 1)) {
    IFERR(mkdir(path.substr(0, position).c_str(), mode)) {
      if (errno != EEXIST) return -1;
    }
  }
  return 0;
}
