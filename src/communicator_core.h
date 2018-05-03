#ifndef COMMUNICATOR_CORE_H_
#define COMMUNICATOR_CORE_H_

/// This header defines functions to communicate with web server and scheduler.
/// All functions are thread-safe.

// ----- Web server -----

/// Query judge / kill / rejudge requests
bool QueryRequest(const /*Request type*/& req);

/// Fetch submission after a timestamp
void FetchProblemSubmissions(int prod_id, /*time+id*/ timestamp);

/// Check testdata consistency + fetch testdata of a problem
void FetchTestdata(int prob_id);

/// Check problem settings consistency + fetch
void FetchProblemSpec(int prob_id);

/// Send judge results or confirmation
void SendMsg(const /*Message type*/& result);

// ----- Scheduler -----

/// Make a kill request
void RequestKill(int submission_id);

/// Make a judge request
void RequestJudge(int submission_id);

/// Wait for messages for timeval ms
int GetMsg(/*Message type*/& result, long& timeval);

#endif
