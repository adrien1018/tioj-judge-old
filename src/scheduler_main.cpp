
// ----- Scheduling -----

// some structures to maintain job queue

void JobEnqueue(/*Job type*/ job) {

}

/*Job type*/ JobDequeue() {

}

// ----- Execution -----

// some structure to maintain running jobs and resources (including uid)

bool KillJob(/*Job handle*/ job) {
  if (/*job not exist*/) return false;
  // kill
  // sigsuspend SIGCHLD (or just let it go?)
  return true;
}

/// Call grader to execute the topmost job in the queue
void StartJob() {
  // JobDequeue
}

/// Check if there are jobs to run now; if yes, start it
bool CheckJob() {

}

/// Handle requests from communicator
void HandleRequest(const /*Request type*/& req) {
  // ...
  CheckJob();
}

volatile bool restart = false;

void SignalHandler(int signo) {
  switch (signo) {
    case SIGCHLD: {
      while (/*wait for each child*/) {
        // close fd
        // send judge finished
        // resolve jobs dependencies
        CheckJob();
      }
      break;
    }
    case SIGHUP: {
      restart = true;
      break;
    }
    case SIGINT: {
      // kill all jobs
      exit(2);
    }
    default: {
      // It shouldn't be here!
    }
  }
}

/// Main loop
void Start() {
  while (!restart) {
    // unblock signal
    // select (from all fds of graders and communicator)
    // block signal
    // if interrupted by signal, continue
    if (/*grader*/) {
      SendJudgeMsg(GetJudgeMsg(/*fd*/));
    }
    if (/*communicator*/) {
      HandleRequest(/*request*/);
    }
  }
  // wait for all jobs to complete
}

void ParseArgs(int argc, char** argv) {
  // Command line parsing
}

int main(int argc, char** argv) {
  ParseArgs(argc, argv);

  // Signal handling
}
