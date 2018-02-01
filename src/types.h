#ifndef TYPES_H_
#define TYPES_H_

#define _GNU_SOURCE

#include <sched.h>
#include <sys/types.h>
#include <linux/taskstats.h>

#include <string>
#include <vector>

struct Command {
  struct Mount {
    std::string path, mountpoint;
    // read-only mount
    // path is absolute, mountpoint is relative to box
    // useful when the task needs dynamic libs
  };
  struct Requirement {
    std::string file, file_in_box;
    // files to hardlink into the box
    // file is absolute, file_in_box is relative to box
  };
  std::string box_dir;
  std::vector<std::string> command;
  std::vector<Mount> mounts;
  std::vector<Requirement> requirements;
  std::vector<int> syscall_whitelist;
  long long time_limit;     // microseconds
  long long rss_limit;      // bytes
  long long vss_limit;      // bytes
  long long filesize_limit; // bytes
  uid_t uid;
  cpu_set_t mask;
};

struct Task {
  struct OpenFile {
    std::string filename;
    size_t proc;
    int fd, mode;
  };
  struct Pipe {
    size_t read_proc, write_proc;
    int read_fd, write_fd;
  };
  std::vector<Command> commands;
  std::vector<OpenFile> files;
  std::vector<Pipe> pipes;
  int process_limit;
};

typedef std::vector<struct taskstats> RunResult;

#endif
