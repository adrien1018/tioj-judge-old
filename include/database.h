#ifndef FILESTRUCT_H_
#define FILESTRUCT_H_

/// This header contains helper functions to maintain and read from judge database

#include <vector>

#include "config.h"
#include "mysql_helper.h"

// ----- General -----

// Initialize an MySQLSession and perform database/table check (if true)
void InitMySQLSession(MySQLSession&, bool check = true);

// ----- Problem settings -----

class Arguments : public std::vector<std::string> {
public:
  using std::vector<std::string>::vector;
  using std::vector<std::string>::operator=;
};

// Fixed-point decimal value to represent score
class ScoreInt {
 private:
  long long val_;
 public:
  static const int kBase;

  ScoreInt();
  ScoreInt(double);
  ScoreInt(long double);
  ScoreInt(int64_t); // direct conversion, not by value

  ScoreInt& operator=(double);
  ScoreInt& operator=(long double);
  ScoreInt& operator=(int64_t);

  ScoreInt& operator+=(const ScoreInt&);
  ScoreInt& operator*=(const ScoreInt&);

  operator long double() const;
  operator int64_t() const;

  friend ScoreInt operator+(ScoreInt, const ScoreInt&);
  friend ScoreInt operator*(ScoreInt, const ScoreInt&);
};

ScoreInt operator+(ScoreInt, const ScoreInt&);
ScoreInt operator*(ScoreInt, const ScoreInt&);

// structure of problem settings
struct ProblemSettings {
  struct CompileSettings {
    Language lang;
    // additional compile flags
    Arguments args;
    CompileSettings();
  };

  int problem_id;

  bool is_one_stage;

  // whether to do code checking and the program language
  CompileSettings code_check_compile;

  struct CustomLanguage {
    CompileSettings compile;
    // if true, an additional common file is provided.
    //  It (instead of submitted code) will be compiled as interpreter
    //  and used in the execution stage.
    //  (submitted file will be passed as an argument)
    bool as_interpreter;
    double tl_a, tl_b, ml_a, ml_b; // TL & ML adjustments
    // syscall adjustments; true for blacklist
    std::vector<std::pair<std::string, bool>> syscall_adj;
    CustomLanguage();
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

  // times of execution (N), used by (old/new)interactive and multiphase
  int execution_times;

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
  enum EvaluationType {
    kEvalSkip = 0, // skip evaluation stage; use output of lib instead
    kEvalNormal = 1,
    kEvalDefaultProgram = 2,
    kEvalOldSpecialJudge = 3,
    kEvalSpecialJudge = 4
  } evaluation_type;

  // options of old-style special judge or skipped evaluation
  enum EvalOutputFormat {
    kEvalFormatZero = 1, // 0 for AC
    kEvalFormatFloat = 2, // output is in [0, 1], 1 for AC
    kEvalFormatFloatNonzero = 3 // output is in [0, 1], nonzero for AC
    // 2 & 3 will use a password to identify
  } evaluation_format;
  long long password;

  // compilation settings of evaluation program
  // if use default program, then lang = null, args is options of evaluation
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
    ResultColumn();
  };
  std::vector<ResultColumn> evaluation_columns;

  // if true, then those TLE/MLE/RE will still be evaluated
  bool evaluate_nonnormal;

  enum ScoringType {
    kScoringNormal = 1,
    kScoringWeighted = 2,
    kScoringSpecial = 3
  } scoring_type;
  // compilation settings of evaluation program
  //  also used by 1-stage mode
  CompileSettings scoring_compile;
  std::vector<ResultColumn> scoring_columns;

  // file count per testdata (usually 2: input and output)
  int file_per_testdata;

  struct TestdataFile {
    int id;
    // path of file in sandbox
    std::string path;
    TestdataFile();
  }; // these files will appear in both exec and eval stage
  std::vector<TestdataFile> testdata_files;

  struct CommonFile {
    enum Usage {
      kLib = 1,
      kIntepreter = 2,
      kCustomStage = 3,
      kOther = 4
    } usage;
    // used only if usage == kLib
    Language lib_lang;
    // if usage == kLib, it is stage number (for multiphase separated)
    // if usage == kIntepreter, it is custom language ID
    // if usage == kCustomStage, it is stage ID
    int id;
    // used only if usage == kOther
    bool stages[4];
    int file_id;
    // used only if (usage == kLib and not CFinteractive/Outputonly)
    //  or usage == kOther
    std::string path;
    CommonFile();
  };
  std::vector<CommonFile> common_files;

  // used by 1-stage mode only; if true, kill old execution when new
  //  judge request of the same problem arrived
  bool kill_old;

  // fully-customized stage
  std::vector<std::pair<int, CompileSettings>> custom_stage;

  struct Testdata {
    long long time_lim, memory_lim; // us, KB
    // Length = file_per_testdata; all files in a problem are uniquely numbered,
    //  to avoid retransmission and for convenience of reordering testdata
    std::vector<int> file_id;
    // Additional args when executing
    Arguments args;
    Testdata();
  };
  std::vector<Testdata> testdata;

  // score ranges
  struct ScoreRange {
    ScoreInt score;
    // testdata in this range, increasing
    std::vector<int> testdata;
    ScoreRange();
  };
  std::vector<ScoreRange> ranges;

  // TODO: Change to event counter (web server should be modified)
  long long timestamp;

  ProblemSettings();
};

// Get timestamp of a problem; if not exist, throw invalid_argument
long long GetProblemTimestamp(MySQLSession&, int);

// QUESTION: Is this necessary?
// Check if settings is valid
// bool IsValidProblemSettings(const ProblemSettings&);

// Get settings of a problem; if not exist, throw invalid_argument
ProblemSettings GetProblemSettings(MySQLSession&, int);

// Update settings of a problem
void UpdateProblemSettings(MySQLSession&, const ProblemSettings&);

#endif
