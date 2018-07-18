#ifndef COMMUNICATOR_CORE_H_
#define COMMUNICATOR_CORE_H_

/// This header defines functions to communicate with web server and scheduler
///  and necessary locks / structures maintaining communicator status,
/// All functions are thread-safe.

struct ServerMessage {
  enum Type {
    kJudgeRequest = 1,
    // Testdata generation is the same as judge request, except that
    //  it needs to acquire per-problem lock
    kGenTestdata = 2,
    kKillRequest = 3,
    kDeleteRequest = 4
  } type;
  int problem_id;
  int submission_id;
  // QUESTION: Is this necessary?
  bool judge_by_problem;
};

struct GraderMessage {
  enum Type {
    kTestdataResult = 1,
    kSubmissionResult = 2,
    kFinished = 3
  };
  int submission_id;
  int testdata_id;
};

/// Initialize (called when communicator starts)
void InitCore();

/// Query judge / kill / testdata generation requests (synchronous)
/// If fd >= 0, this function will return false when read to fd will not block.
bool GetServerMsg(ServerMessage&, int fd = -1);

/// Get a message from grader (synchronous)
bool GetGraderMsg(GraderMessage&, int fd = -1);

/// Send a message to web server (asynchronous)
void SendMsg(GraderMessage&);

/// Update problem settings & testdata & submissions (synchronous)
void FetchProblem(int);

/// Request scheduler to kill a submission (asynchronous)
void RequestKill(int);

/// Request scheduler to judge a submission (asynchronous)
/// judge_by_problem is represented as negative submission id, where
///  problem_id = -submission_id
void RequestJudge(int);

/// Problem / submission lock maintenance
/// Note that problem locks and submission locks are independent;
///  it is necessary to check whenever updating a submission.
void LockProblem(int id);
void LockSubmission(int id);
void UnlockProblem(int id);
void UnlockSubmission(int id);

#endif
