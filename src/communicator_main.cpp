#include "communicator_core.h"

#include <signal.h>
#include <queue>
#include <vector>
#include <thread>
#include <condition_variable>

// ----- constants (specified in opts) -----
int kWorkerThread = 4;
// time from last submission to be compressed, in hours; -1 to disable
int kCompressThreshold = 72;

// ----- Worker threads status -----
struct ThreadInfor {
  int problem_id, submission_id;
  int status; // -1: interrupted, 0: fetching, 1: waiting for grader
};
std::mutex status_mtx;
std::condition_variable status_cv;
std::vector<ThreadInfor> thread_status;

// ----- Request queue -----
std::mutex queue_mtx;
std::condition_variable queue_cv;
std::queue<ServerMessage> request_queue;

// ----- Judge threads -----

/// Request to kill a submission
void KillSub(int submission_id) {
  // if have not been send to scheduler, modify its status directly
}

/// Judge request handler
void HandleRequest(const ServerMessage& msg) {
}

/// Handle requests
void WorkerThread() {
  while (true) {
    ServerMessage msg;
    {
      std::unique_lock<std::mutex> lock(queue_mtx);
      queue_cv.wait(lock, []{ return !request_queue.empty(); });
      msg = request_queue.front();
      request_queue.pop();
    }
    HandleRequest(msg);
  }
}

/// Handle compressions
void CompressionThread() {

}

std::mutex restart_mtx;
volatile bool restart = false;
volatile bool killall = false;

/// In signal handling thread
void SignalHandler(int signo) {
  switch (signo) {
    case SIGHUP: {
      restart_mtx.lock();
      restart = true;
      restart_mtx.unlock();
      break;
    }
    case SIGINT: {
      restart_mtx.lock();
      restart = true;
      killall = true;
      restart_mtx.unlock();
      break;
    }
    default: {
      // It shouldn't be here!
    }
  }
}

/// Signal handling thread
void SignalThread() {

}

/// Handle messages from scheduler
void MessageThread() {
  while (true) {
    GraderMessage msg;
    bool ret = GetGraderMsg(msg/*, */);
    {
      std::lock_guard<std::mutex> lock(restart_mtx);
      if (restart) break;
    }
    if (!ret) continue; // should not happen?
    SendMsg(msg);
  }
}

/// Send kill request to all submissions
void KillAll() {}

void ParseArgs(int argc, char** argv) {
  // Command line parsing
}

int Main(int argc, char** argv) {
  ParseArgs(argc, argv);

  // Signal handling
  InitCore();

  // Spawn threads

  // main loop
  while (true) {
    ServerMessage msg;
    bool ret = GetServerMsg(msg/*, */);
    {
      std::lock_guard<std::mutex> lock(restart_mtx);
      if (restart) break;
    }
    if (!ret) continue; // should not happen?

    { // handle request
      std::lock_guard<std::mutex> lock(queue_mtx);
      request_queue.push(msg);
    }
    queue_cv.notify_one();
  }
  if (killall) KillAll();
  // join all threads
  return 0;
}
