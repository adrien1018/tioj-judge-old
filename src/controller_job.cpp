#include "controller_job.h"

#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include <set>

#include "types.h"
#include "grader.h"

JobGraph job_graph;
std::set<QueueingJob> queueing_jobs;

void ReadSubmission() {
  // read problem settings and submission files from file (kDataPath)
  //
}

Request QueryRequest() {
  // read request from communicator
  // if no request, return immediately
}

void RemoveJobs(const Request& req) {
  // kill running jobs, modify graph and queueing jobs
}

void RunJob(const Job& job/**/) {
  // run a job
}

bool job_finished = false;
bool have_request = false;

void SignalHandler(int signum) {
  if (signum == SIGCHLD) job_finished = true;
  else if (signum == SIGUSR1) have_request = true;
}

void FinishJob(/**/) {
  // send information to communicator
  // modify graph and queue
  // remove other jobs if necessary
}

void RunJobController(const std::vector<int>& avail_cpus) {
  // this function will start with SIGCHLD and SIGUSR1 blocked
  {
    sigset_t used_sig;
    sigemptyset(&used_sig);
    sigaddset(&used_sig, SIGCHLD);
    sigaddset(&used_sig, SIGUSR1);

    struct sigaction act;
    act.sa_handler = SignalHandler;
    act.sa_mask = used_sig;
    act.sa_flags = SA_NOCLDSTOP;
    sigaction(SIGCHLD, &act, nullptr);
    sigaction(SIGUSR1, &act, nullptr);
  }
  while (1) {
    sigset_t empty_set;
    sigemptyset(&empty_set);
    sigsuspend(&empty_set);
    if (have_request) {
      while (1) {
        Request req = QueryRequest();
        if (req.type == Request::kNothing) break;
        if (req.type == Request::kKillRequest) {
          RemoveJobs(req);
        }
      }
    } else { // child finished
      pid_t pid;
      int status;
      while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        FinishJob(/**/);
      }
    }
    if (queueing_jobs.size()/* && threads avaliable now to run *queueing_jobs.begin()*/) {
      RunJob(JobGraph::GetJob(queueing_jobs.begin()->job)/*cpu?*/);
    }
  }
}

