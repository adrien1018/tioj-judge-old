#ifndef ENV_SETUP_H_
#define ENV_SETUP_H_

// this header defines helper functions to:
//  1) read / write settings to kDataPath? (communicator)
//  2) setup submission dir in kBoxPath (communicator)
//  3) add new submissions to job graph (controller)

#include "types.h"

// will this be used?
// Problem ReadProblem(int problem_id);
// void WriteProblem(const Problem& prob);

// communicator will call this first, and fetch user files into submission dir
void SetupSubmissionDir(/*some type?*/);

void AddSubmissionIntoGraph(JobGraph& graph/*,some type?*/);

#endif
