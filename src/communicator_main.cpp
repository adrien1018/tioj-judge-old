#include "communicator_core.h"
#include "filestruct.h"

// ----- Submission / problem lock maintenance -----

enum LockType {
  kReadLock,
  kWriteLock
};

enum LockTarget {
  kSubmissionLock,
  kProblemLock
};

// some structures and mutexs to maintain locks

bool GetLock(LockTarget target, int id, LockType type) {

}

bool ReleaseLock(LockTarget target, int id) {

}

// ----- Judge threads -----

/// Judge request handler
void Judge(int submission_id) {
  // acquire locks
}

/// Rejudge request handler
void Rejudge(int submission_id) {
  RequestKill(submission_id);
  // ...
  Judge(submission_id);
}

/// Need a mutex
volatile bool restart = false;

/// Main loop, run in main thread
void Start() {
  while (true) {
    // acquire restart mutex
    if (restart) break;
    // release restart mutex

    if (QueryRequest(/**/)) {
      if (/*kill request*/) {
        RequestKill(/*submission id*/);
      } else {
        // spawn thread to handle it
      }
    }
    long timeout = /**/;
    while (timeout != 0 && GetMsg(/**/, timeout)) {
      SendMsg(/**/);
    }
  }
  // GetMsg until scheduler ends (EOF)
}

/// In signal handling thread
void SignalHandler(int signo) {
  switch (signo) {
    case SIGHUP: case SIGINT: {
      // acquire mutex
      restart = true;
      // release mutex
      break;
    }
    default: {
      // It shouldn't be here!
    }
  }
}

/// Main loop in signal handling thread
void SignalThread() {

}

void ParseArgs(int argc, char** argv) {
  // Command line parsing
}

int main(int argc, char** argv) {
  ParseArgs(argc, argv);

  // Signal handling

  // Spawn signal handling thread
  Start();
}
