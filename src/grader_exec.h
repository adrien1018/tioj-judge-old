#ifndef GRADER_EXEC_H_
#define GRADER_EXEC_H_

/// This header defines functions to run a stage.


void RunPreExecStage(int submission_id);
void RunExecEvalStage(int submission_id, int testdata_id);
void RunScoringStage(int submission_id);
void RunOneStage(int submission_id);

#endif
