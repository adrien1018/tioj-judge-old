#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <string>
#include <vector>

struct Command {
  struct Mount {
    std::string path, mountpoint;
    // read-only mount, mountpoint is relative to box
    // useful when the task needs dynamic libs
  };
  struct Requirement {
    std::string file, file_in_box;
    // files (usually exes, eg. python) to hardlink into the box
  };
  std::vector<std::string> command;
  std::vector<Mount> mounts;
  std::vector<Requirement> requirements;
  std::vector<int> syscall_whitelist;
  long long time_limit;     // microseconds
  long long rss_limit;      // bytes
  long long vss_limit;      // bytes
  long long filesize_limit; // bytes
  int uid;
};

struct Task {
  struct OpenFile {
    std::string filename;
    size_t proc;
    int fd, mode; // fd < 0: not open
    bool in_box;  // whether the file is visible inside the box
    std::string filename_in_box;
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

#endif
