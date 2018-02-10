#ifndef TYPES_H_
#define TYPES_H_

// This header defines types used in the judging process.

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sched.h>
#include <sys/types.h>
#include <linux/taskstats.h>

#include <string>
#include <vector>

typedef std::vector<std::string> Arguments;

struct ResLimit {
  static const int kNoLim = -1;  // Constant indicating unlimited
  long long time_limit;      // microseconds
  long long rss_limit;       // bytes
  long long vss_limit;       // bytes
  long long stack_limit;     // bytes
  long long filesize_limit;  // bytes
  int process_limit;
};

class LimAdj {
public:
  LimAdj(double f, long long c) : factor(f), constant(c) {}
  double factor; long long constant;
  template <class T> T operator()(T val) const {
    if (val == ResLimit::kNoLim || factor < 0)
      return ResLimit::kNoLim;
    T result = static_cast<T>(factor * val) + constant;
    return result < 0 ? 0 : result;
  }
};

struct Environment {
  struct PathPair {
    std::string path_org, path_box;
    // represents a pair of file/directory across the sandbox
    //  path_org is absolute or relative to submission dir
    //  path_box is relative to sandbox
    // probably contain some variables in settings
    //  e.g. ${INPUT} ${OUTPUT} (path to input/output testdata file)
    //       ${USERx} ${ASSETx} (path to the x-th file provided by user or problen setter)
    //       ${GCC} ${PYTHON}   (path to some compiler / interpreter)
    //  will be automatically converted to real path by controller
  };
  struct Requirement {
    PathPair file;
    bool hardlink;
    mode_t permission;
  };
  std::vector<PathPair> mounts;  // read-only bind mounts
                                 // useful when the task needs dynamic libs
  std::vector<Requirement> requirements; // files to hardlink/copy into the box
  std::vector<PathPair> results;  // files to move out of the box when finished
  std::vector<std::string> syscall_whitelist;
  mode_t box_permission;
};

struct Program {
  Arguments command;
  Environment env;

  ResLimit limits;

  std::string submission_dir, box_dir;
  uid_t uid;
  cpu_set_t mask;
};

struct ParTask {
  struct OpenFile {
    std::string filename; // absolute or relative to submission dir
    size_t proc;
    int fd, mode;
  };
  struct Pipe {
    size_t read_proc, write_proc;
    int read_fd, write_fd;
  };
  std::vector<Program> programs;
  std::vector<OpenFile> files;
  std::vector<Pipe> pipes;
};

typedef std::vector<ParTask> Task;

typedef std::vector<struct taskstats> RunResult;
typedef std::vector<std::vector<std::string>> StringTable;

enum Verdict {
  kAC = 0,  // Accepted
  kWA,      // Wrong Answer
  kTLE,     // Time Limit Exceeded
  kMLE,     // Memory Limit Exceeded
  kOLE,     // Output Limit Exceeded
  kRE,      // Runtime Error (killed by signal)
  kNE,      // Non-zero Exit status
  kCE,      // Compilation Error
  kJF       // Judgement Failed (internal error / timeout)
};

struct Problem { // This is what stored in problem metafile,
                 //  not necessary to read completely when evaluating submissions
  struct Asset {
    std::string filename;  // contains no path
    mode_t perm;  //permission when copied into submission dir
  };
  struct LangSettings {
    Task pre_exec_stage;
    Task execution_stage;  // not used in competition mode
    LimAdj time_adj, memory_adj;  // in case that different langs need different limits
  };
  struct Testdata {
    ResLimit limits;
    Arguments options;
    std::vector<int> testdata_group;
  };

  int id;  // problem id
  bool competition;
  std::vector<Asset> assets;

  // uid, mask, submission_dir, box_dir in Task struct are not filled
  std::vector<LangSettings> langs;

  size_t overall_result_column_num;

  // --- used in normal mode ---
  std::vector<Testdata> testdata;
  Task evaluation_stage;
  Task scoring_stage;
  Task post_scoring_stage;
  size_t testdata_result_column_num;

  // --- used in competition mode ---
  Task competition_stage;
};

enum Stage {
  kPreExec = 0,
  kExecution = 1,
  kEvaluation = 2,
  kScoring = 3,
  kPostScoring = 4
};

struct Submission {
  int submission_id;
  int problem_id;
  size_t language;

  static const int kAllTestdata = -1;
  int testdata;  // testdata number; -1: all testdata
  bool incomplete;  // incomplete grading

  // TODO: progress & scheduling
  int priority;  // judging priority
  std::string submission_dir;
};

#endif
