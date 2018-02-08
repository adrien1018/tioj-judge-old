#include "grader.h"
#include "utils.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/mount.h>

// bind mount path to mountpoint (readonly)
int BindMountReadOnly(const std::string& path, const std::string& mountpoint) {
  IFERR(mkdir_recursive(mountpoint.c_str(), 0666))
    return -1;
  IFERR(mount(path.c_str(), mountpoint.c_str(),
              NULL, MS_MGC_VAL | MS_RDONLY | MS_BIND, NULL)) return -1;
  IFERR(mount("none", mountpoint.c_str(),
              NULL, MS_RDONLY | MS_REMOUNT | MS_BIND, NULL)) return -1;
  return 0;
}

// helper function: get full path of PathPair.path_box
inline std::string FullBoxPath(const Program& prog,
                               const Environment::PathPair& path) {
  return ConcatPath(prog.box_dir, path.path_box);
}

// mount directories / copy files
int SetupBox(const Program& cmd) {
  IFERR(mkdir_recursive(cmd.box_dir.c_str(), 0666))
    return -1;
  for (const Environment::PathPair& mnt : cmd.env.mounts) {
    IFERR(BindMountReadOnly(mnt.path_org.c_str(),
                            FullBoxPath(cmd, mnt).c_str())) {
      return -1;
    }
  }
  for (const Environment::Requirement& req : cmd.env.requirements) {
    if (req.hardlink) {
      IFERR(link(req.file.path_org.c_str(),
                 FullBoxPath(cmd, req.file).c_str())) {
        return -1;
      }
    }
    else {
      IFERR(CopyFile(req.file.path_org, FullBoxPath(cmd, req.file),
                     req.permission)) {
        return -1;
      }
    }
  }
  return 0;
}

// umount, move files and remove directories
int ClearBox(const Program& cmd) {
  for (const Environment::PathPair& mnt : cmd.env.mounts) {
    IFERR(umount(FullBoxPath(cmd, mnt).c_str())) {
      return -1;
    }
  }
  for (const Environment::PathPair& mv : cmd.env.results) {
    IFERR(rename(FullBoxPath(cmd, mv).c_str(), mv.path_org.c_str())) {
      if (errno != ENOENT) return -1;
    }
  }
  IFERR(rmdir_recursive(cmd.box_dir.c_str()))
    return -1;
  return 0;
}

int SetupEnvironment(const ParTask& task/* , std::vector<struct cjail_para>& params */) {
  for (const Program& prog : task.programs) SetupBox(prog);
  // TODO: setup pipes, open files, params
  return 0;
}

int ClearEnvironment(const ParTask& task) {
  for (const Program& prog : task.programs) ClearBox(prog);
  // TODO: close files
  return 0;
}

// Run a taskset; all path_org should be absolute now (modified by controller)
RunResult RunGrader(const ParTask& task) {
  SetupEnvironment(task/*, params */);

  RunResult result(task.programs.size());
  // TODO: call sandbox, collect results, copy files out

  ClearEnvironment(task);
  return result;
}
