#ifndef UTILS_H_
#define UTILS_H_

// This header defines some helper functions.

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <string>

#include "types.h"

#define IFERR(x) if((x) < 0)
#define perrf(x, ...) do { fprintf(stderr, x, ##__VA_ARGS__); } while(0)
#define PRINTERR(name) do { fprintf(stderr, "Failed to %s: %s", name, strerror(errno)); } while(0)
#ifdef DEBUG
#define pdebugf(x, ...) do { fprintf(stderr, x, ##__VA_ARGS__); } while(0)
#else
#define pdebugf(x, ...)
#endif

int IsDirectory(const std::string& path);
int IsRegularFile(const std::string& path);
bool IsAbsolutePath(const std::string& path);
bool IsDownwardPath(const std::string& path);
std::string ConcatPath(const std::string& path1, const std::string& path2);

int mkdir_recursive(const char* path, mode_t mode);
int rmdir_recursive(const char* path);

int CopyFile(const std::string& source, const std::string& dest, mode_t perm);

StringTable ReadCsv(const std::string& filename);
int WriteCsv(const StringTable& table, const std::string& filename);

#endif
