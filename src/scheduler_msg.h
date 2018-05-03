#ifndef SCHEDULER_MSG_H_
#define SCHEDULER_MSG_H_

/// This header contains helper functions to communicate with graders and communicator.

/// Get message from a grader
/*Msg type*/ GetJudgeMsg(int fd);

/// Send judge result messages to communicator
void/**/ SendJudgeMsg(const /*Msg type*/&);

/// Get judge / kill requests
/*Request type*/ GetJudgeRequest();

#endif
