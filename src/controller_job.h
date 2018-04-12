#ifndef CONTROLLER_JOB_H_
#define CONTROLLER_JOB_H_

// this header defines job controller
//  read requests from communicator
//  schedule all jobs

struct Request {
  enum Type {
    kNothing = 0,
    kNewJob,
    kKillRequest
  } type;
  int submission_id;  // used in kill request
  int testdata;  // Submission::kAllTestdata can fit here
};

// struct RunningJob (?

void RunJobController();

#endif
