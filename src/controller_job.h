#ifndef CONTROLLER_JOB_H_
#define CONTROLLER_JOB_H_

#include <queue>

#include "types.h"
#include "grader.h"

extern JobGraph job_graph;
extern std::priority_queue<QueueingJob> queueing_jobs;

void run_job_controller();

#endif
