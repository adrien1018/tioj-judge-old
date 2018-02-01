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

// mount directories / hardlink files
int SetupBox(const Command& cmd) {
  IFERR(mkdir_recursive(cmd.box_dir.c_str(), 0666))
    return -1;
  for (const Command::Mount& mnt : cmd.mounts) {
    IFERR(BindMountReadOnly(mnt.path.c_str(),
                            ConcatPath(cmd.box_dir, mnt.mountpoint).c_str())) {
      return -1;
    }
  }
  for (const Command::Requirement& req : cmd.requirements) {
    IFERR(link(req.file.c_str(),
               ConcatPath(cmd.box_dir, req.file_in_box).c_str())) {
      return -1;
    }
  }
  return 0;
}

// umount and remove directories
int ClearBox(const Command& cmd) {
  for (const Command::Mount& mnt : cmd.mounts) {
    IFERR(umount(ConcatPath(cmd.box_dir, mnt.mountpoint).c_str())) {
      return -1;
    }
  }
  IFERR(rmdir_recursive(cmd.box_dir.c_str()))
    return -1;
  return 0;
}

int SetupEnvironment(const Task& task/* , std::vector<struct cjail_para>& params */) {
  for (const Command& cmd : task.commands) SetupBox(cmd);
  // TODO: setup pipes, open files, params
}

int ClearEnvironment(const Task& task) {
  for (const Command& cmd : task.commands) ClearBox(cmd);
  // TODO: close files
}

RunResult RunGrader(const Task& task) {
  SetupEnvironment(task/*, params */);

  RunResult result(task.commands.size());
  // TODO: call sandbox, collect results

  ClearEnvironment(task);
  return result;
}
