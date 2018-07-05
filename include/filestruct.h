#ifndef FILESTRUCT_H_
#define FILESTRUCT_H_

/// This header contains helper functions to maintain and read from judge file structures

#include <vector>

#include "config.h"
#include "mysql_helper.h"

class Arguments : public std::vector<std::string> {
  using std::vector<std::string>::vector;
};

// structure of problem setting
struct ProblemSettings {
  struct CompileSettings {
    Language lang;
    // additional compile flags
    Arguments args;
  };

  int problem_id;

  // whether to do code checking and the program language
  Language check_code_lang;

  struct CustomLanguage {
    CompileSettings compile;
    double tl_a, tl_b, ml_a, ml_b; // TL & ML adjustments
    // syscall adjustments; true for blacklist
    std::pair<std::string, bool> syscall_adj;
  };
  // list of customized languages
  std::vector<CustomLanguage> custom_lang;

  enum ExecutionType {
    kExecNormal = 1,              // 1 user file
    kExecOldInteractive = 2,      // 1 user file, 1 judge header
    kExecInteractive = 3,         // 1 user impl, 1 judge file+header (dif. argv)
    kExecMultiPhaseSeparated = 4, // N user impl, N judge file+header
    kExecOutputOnly = 5,          // (testdata#) user files, 1 judge file (dif. argv)
    kExecCFInteractive = 6        // 1 user file, 1 judge file
  } execution_type;

  // times of execution (N), used by interactive and multiphase
  int execution_times;

  // lib filename in each stage, length = N if multiphase,
  //  length = 1 if (old/new)interactive, 0 otherwise
  std::vector<std::string> lib_name;

  // compile settings of lib file
  // used by output only and CF interactive
  CompileSettings lib_compile;

  // if true, then the input/output would be pipe instead of regular file
  //  (to disable seeking)
  // QUESTION: FIFO or regular pipe?
  bool pipe_in, pipe_out;

  // if true, stop at first non-AC testdata of each subtask
  bool partial_judge;

  // evaluation types
  static const int kEvalDefaultProgram = 0x10;
  static const int kEvalOldSpecialJudge = 0x20;
  static const int kEvalSpecialJudge = 0x30;
  static const int kEvalTypeMask = 0xf0;

  // options of old-style evaluation
  static const int kEvalOptNormal = 0x1; // 0 for AC
  static const int kEvalOptFloat = 0x2; // output is in [0, 1], 1 for AC
  static const int kEvalOptFloatNonzero = 0x3; // output is in [0, 1], nonzero for AC
  static const int kEvalOptSkip = 0x8; // skip evaluation stage; use output of lib instead
  static const int kEvalOptMask = 0xf;

  // bitwise-or of above options
  int evaluation_type;
  // compilation settings of evaluation program
  // if use default program, then lang is not used, args is options of evaluation
  CompileSettings evaluation_compile;

  struct ResultColumn {
    enum ColumnType {
      kScoreFloat = 1, // 64-bit signed integer / 10^6, used to represent score
      kInteger = 2,    // 64-bit signed integer
      kFloat = 3,      // long double
      kShortText = 4,  // text not longer than 64 bytes
      kText = 5        // long text, must be the last column
      // kFile = 6     // file
      // QUESTION: should this exist?
    } type;
    bool visible; // if true, it will be send to the web server
  };
  std::vector<ResultColumn> eval_columns;

  // if true, then those TLE/MLE/RE will still be evaluated
  bool eval_nonnormal;

  enum ScoringType {
    kScoringNormal = 1,
    kScoringWeighted = 2,
    kScoringSpecial = 3
  } scoring_type;
  CompileSettings scoring_compile;

  // file count per testdata (usually 2: input and output)
  int file_per_testdata;

  struct FileInSandbox {
    // stage only used by common file
    int id, stage;
    // path of file in sandbox
    std::string path;
  };
  std::vector<FileInSandbox> testdata_file_path, common_file_path;

  // TODO: Sandbox settings struct
  // std::vector<std::pair<int, SandboxSettings>> custom_stage;

  long long timestamp;
};

// Initialize an MySQLSession and perform database/table check
void InitMySQLSession(MySQLSession&);

#endif
