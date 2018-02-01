#ifndef UTILS_H
#define UTILS_H

#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cstdio>
#include <string>
#include <cstring>

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
int mkdir_recursive(const std::string& path, mode_t mode);

#endif
