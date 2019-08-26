#ifndef DATABASE_H_
#define DATABASE_H_

/// This header contains helper functions to maintain and read from judge database

#include "settings.h"
#include "mysql_helper.h"

// Initialize an MySQLSession and perform database/table check (if true)
void InitMySQLSession(MySQLSession&, bool check = true);

// Get timestamp of a problem; if not exist, throw invalid_argument
long long GetProblemTimestamp(MySQLSession&, int);

// Check if a problem is 1-stage and kill_old
bool IsJudgeByProblem(MySQLSession&, int);

// QUESTION: Is this necessary?
// Check if settings is valid
// bool IsValidProblemSettings(const ProblemSettings&);

// Get settings of a problem; if not exist, throw invalid_argument
ProblemSettings GetProblemSettings(MySQLSession&, int);

// Update settings of a problem
void UpdateProblemSettings(MySQLSession&, const ProblemSettings&);

// a lot more...

#endif
