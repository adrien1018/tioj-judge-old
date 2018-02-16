#ifndef TYPES_H_
#define TYPES_H_

// This header defines types used in the judging process.

// TODO: move some types to controller or grader

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sched.h>
#include <sys/types.h>

#include <list>
#include <string>
#include <vector>

// #include "cjail.h"

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
  template <class T> T operator()(T val) const;
};

struct Environment {
  struct PathPair {
    std::string path_org, path_box;
    // represents a pair of file/directory across the sandbox
    //  path_org is absolute
    //  path_box is relative to sandbox or blank (= same as path_org)
    // probably contain some variables in settings
    //  e.g. ${INPUT} ${OUTPUT} (path to input/output testdata file)
    //       ${SUBDIR}          (path to the submission dir)
    //       ${USER_x}          (NAME of the x-th file provided by user)
    //       ${stage_x}         (NAME of the x-th asset for a certain stage)
    //       ${gcc} ${python3}  (path to some compiler / interpreter)
    //       ${METAFILE}        (name of metafile
    //                           (contains information about submission, inside sandbox))
    //  will be automatically converted to real path by controller
    //  thus character '$' need to be escaped if needed
  };
  struct Requirement {
    PathPair file;
    bool hardlink;
    mode_t permission;
  };
  std::vector<PathPair> mounts;  // read-only bind mounts
                                 // useful when the task needs dynamic libs
  std::vector<Requirement> requirements;  // files to hardlink/copy into the box
  std::vector<PathPair> results;  // files to move out of the box when finished
  std::vector<std::string> syscall_whitelist;
  mode_t box_permission;
};

// TODO: Environment variables?
struct Program {
  Arguments command;
  // probably contain some variables
  //  e.g. all mentioned at Environment::PathPair
  //       ${RUNNUM}   (# of testdata in multiphase or output only)
  //  will be automatically converted to real value by controller
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
  int processor_count; // number of processor needed
};

typedef std::list<ParTask> Task; // use list so that tasks can be merged easily
typedef std::vector<struct taskstats/*struct cjail_result*/> RunResult;

// --- used by controller ---

typedef std::vector<std::vector<std::string>> StringTable;

enum Verdict {
  kVerdictAC = 0,  // Accepted
  kVerdictWA,      // Wrong Answer
  kVerdictTLE,     // Time Limit Exceeded
  kVerdictMLE,     // Memory Limit Exceeded
  kVerdictOLE,     // Output Limit Exceeded
  kVerdictRE,      // Runtime Error (killed by signal)
  kVerdictNE,      // Non-zero Exit status
  kVerdictCE,      // Compilation Error
  kVerdictJF       // Judgement Failed (internal error / timeout)
};

struct Problem { // This is what stored in problem metafile,
                 //  not necessary to read completely when evaluating submissions
  struct File {
    std::string filename;  // contains no path
    mode_t perm;  // permission when copied into submission dir
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
  std::vector<File> assets, user_files;

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
  kStagePreExec = 0,
  kStageExecution = 1,
  kStageEvaluation = 2,
  kStageScoring = 3,
  kStagePostScoring = 4,
  kStageCompetition = -1
};

struct Submission {
  int submission_id;
  int problem_id;
  size_t language;

  static const int kAllTestdata = -1;
  int testdata;  // testdata number; -1: all testdata
  bool incomplete;  // incomplete grading

  int priority;  // judging priority (0-255, default 127)
  std::string submission_dir;
};

struct Job {
  Task tasks;
  int serial_num;  // since the same submission may be rejudged, an unique
                   //  serial number is needed to avoid unknown conflicts
  int submission_id;
  Stage stage;
  int testdata;  // exec and eval stage
  int priority;  // scheduling priority, small is prior
                 //  0-16777215, top 8 bits are judging priority
};

class JobGraph {
private:
  class JobGraphVertex {
    friend class JobGraph;
    Job job_;
    std::vector<std::list<JobGraphVertex>::iterator> in_edges_, out_edges_;
  public:
    JobGraphVertex(const Job& j) : job_(j) {}
  };
  std::list<JobGraphVertex> vertices_;
public:
  typedef std::list<JobGraphVertex>::iterator VertexPtr;
  JobGraph() {}

  VertexPtr InsertVertex(const Job& job);
  void AddEdge(VertexPtr from, VertexPtr to);
  std::vector<VertexPtr> RemoveVertex(VertexPtr ptr);  // return those have no in edges

  static const Job& GetJob(VertexPtr ptr);
  size_t size() const;
};

class QueueingJob {
public:
  QueueingJob(JobGraph::VertexPtr j) : job(j) {}
  JobGraph::VertexPtr job;
  bool operator<(const QueueingJob& rhs) const;
};

#endif
