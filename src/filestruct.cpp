#include "filestruct.h"

#include "config.h"

// MySQL table schema
const DatabaseTable kProbSettingTable("problem_settings",
    { //*: used by both stages
      {"problem_id",               "INT",   "UNIQUE NOT NULL"}, //*
      {"is_one_stage",             "BOOL",  "NOT NULL"},        //*
      {"check_code_lang",          "INT",   "NOT NULL"},
      {"custom_code_lang",         "BOOL",  "NOT NULL"},
      {"execution_type",           "INT",   "NOT NULL"},
      {"pipe_in",                  "INT",   "NOT NULL"},
      {"pipe_out",                 "INT",   "NOT NULL"},
      {"partial_judge",            "BOOL",  "NOT NULL"},
      {"evaluation_type",          "INT",   "NOT NULL"},
      {"custom_evaluation",        "BOOL",  "NOT NULL"},
      {"evaluate_nonnormal",       "BOOL",  "NOT NULL"},
      {"scoring_type",             "INT",   "NOT NULL"},        //*
      {"file_per_testdata",        "INT",   "NOT NULL"},
      {"testdata_file_in_sandbox", "BOOL",  "NOT NULL"},
      {"common_file",              "INT",   "NOT NULL"},        //*
      {"custom_stage",             "BOOL",  "NOT NULL"},        //*
      {"kill_old",                 "BOOL",  "NOT NULL"},        //1-stage only
      {"timestamp",                "DATETIME(3)", "NOT NULL"},
    }, {}, "PRIMARY KEY (problem_id)");
const DatabaseTable kProbExtraAttrTable("problem_extra_attr",
    {
      {"problem_id", "INT", "NOT NULL"},
      {"attr_id",    "INT", "NOT NULL"},
      {"item_id",    "INT", "NOT NULL"},
      {"text",       "VARCHAR(8192)", "NOT NULL"}
    }, {{"problem_id"}},
    "PRIMARY KEY (problem_id, attr_id, item_id), "
    "FOREIGN KEY (problem_id) REFERENCES problem_settings(problem_id)");
const DatabaseTable kTestdataTable("testdata",
    {
      {"problem_id",  "INT", "NOT NULL"},
      {"testdata_id", "INT", "NOT NULL"},
      {"file_id",     "INT", "NOT NULL"},
      {"timestamp",   "DATETIME(3)", "NOT NULL"}
    }, {{"problem_id"}, {"problem_id", "testdata_id"}},
    "PRIMARY KEY (problem_id, testdata_id, file_id), "
    "FOREIGN KEY (problem_id) REFERENCES problem_settings(problem_id)");
const DatabaseTable kRangeTable("ranges",
    {
      {"problem_id", "INT",    "NOT NULL"},
      {"range_id",   "INT",    "NOT NULL"},
      {"score",      "BIGINT", "NOT NULL"},
    }, {{"problem_id"}},
    "PRIMARY KEY (problem_id, range_id), "
    "FOREIGN KEY (problem_id) REFERENCES problem_settings(problem_id)");
const DatabaseTable kRangeMappingTable("range_mapping",
    {
      {"problem_id",  "INT", "NOT NULL"},
      {"testdata_id", "INT", "NOT NULL"},
      {"range_id",    "INT", "NOT NULL"}, // index necessary???
    }, {{"problem_id"}, {"problem_id", "range_id"}, {"problem_id", "testdata_id"}},
    "FOREIGN KEY (problem_id, testdata_id) REFERENCES testdata(problem_id, testdata_id), "
    "FOREIGN KEY (problem_id, range_id) REFERENCES ranges(problem_id, range_id)");
const DatabaseTable kSubmissionTable("submissions",
    {
      {"submission_id", "INT",        "NOT NULL"},
      {"problem_id",    "INT",        "NOT NULL"},
      {"user_id",       "INT",        "NOT NULL"},
      {"custom_lang",   "BOOL",       "NOT NULL"},
      {"lang_id",       "INT",        "NOT NULL"},
      {"verdict",       "INT",        "NOT NULL"},
      {"time",          "BIGINT",     "NOT NULL"},
      {"memory",        "BIGINT",     "NOT NULL"},
      {"score",         "BIGINT",     "NOT NULL"},
      {"custom_field",  "MEDIUMTEXT", "NOT NULL"}
    }, {{"problem_id"}},
    "PRIMARY KEY (submission_id), "
    "FOREIGN KEY (problem_id) REFERENCES problem_settings(problem_id)");
const DatabaseTable kTestdataResultTable("testdata_results",
    {
      {"submission_id", "INT",        "NOT NULL"},
      {"testdata_id",   "INT",        "NOT NULL"},
      {"verdict",       "INT",        "NOT NULL"},
      {"real_time",     "BIGINT",     "NOT NULL"},
      {"cpu_time",      "BIGINT",     "NOT NULL"},
      {"memory_rss",    "BIGINT",     "NOT NULL"},
      {"memory_vss",    "BIGINT",     "NOT NULL"},
      {"custom_field",  "MEDIUMTEXT", "NOT NULL"}
    }, {{"submission_id"}},
    "PRIMARY KEY (submission_id, testdata_id), "
    "FOREIGN KEY (submission_id) REFERENCES submissions(submission_id)");

ProblemSettings::CompileSettings::CompileSettings() :
    lang(kLangNull), args() {}

// Default problem settings
ProblemSettings::ProblemSettings() :
    problem_id(0),
    is_one_stage(false), // 4-stage mode
    check_code_lang(kLangNull), // no code checking
    custom_lang(), // not used
    execution_type(ProblemSettings::kExecNormal), // batch judge
    execution_times(1), // not used
    lib_name(), lib_compile(), // not used
    pipe_in(false), pipe_out(false), // read from file
    partial_judge(false), // judge whole problem
    evaluation_type(ProblemSettings::kEvalDefaultProgram), // normal judge
    evaluation_compile(), // no args: same as --line --string
    evaluation_columns(), // no additional columns
    scoring_type(ProblemSettings::kScoringNormal), // normal scoring
    file_per_testdata(2), file_common_cnt(0), // no additional files
    testdata_file_path(), common_file_path(),
    kill_old(false), // not used
    // custom_stage(),
    timestamp(0) {}

// initialze statics
const int ProblemSettings::kEvalDefaultProgram = 0x10;
const int ProblemSettings::kEvalOldSpecialJudge = 0x20;
const int ProblemSettings::kEvalSpecialJudge = 0x30;
const int ProblemSettings::kEvalTypeMask = 0xf0;
const int ProblemSettings::kEvalOptNormal = 0x1;
const int ProblemSettings::kEvalOptFloat = 0x2;
const int ProblemSettings::kEvalOptFloatNonzero = 0x3;
const int ProblemSettings::kEvalOptSkip = 0x8;
const int ProblemSettings::kEvalOptMask = 0xf;

// QUESTION: Should sess be global?
void InitMySQLSession(MySQLSession& sess) {
  using namespace mysqlx;
  using std::cout;
  if (!IsValidName(kDatabaseName)) {
    cout << "Invalid database name: " << kDatabaseName << '\n';
    exit(1);
  }
  try { // use login specified in config.cpp
    sess.Start(kMySQLHostname, kMySQLPort, kMySQLUsername, kMySQLPassword);
  } catch (const Error& err) {
    cout << "Error while connecting to MySQL\n" << err << '\n';
    exit(1);
  }
  try {
    sess.ChangeDatabase(kDatabaseName);
  } catch (const Error& err) {
    cout << "Unexpected error\n" << err << '\n';
    exit(1);
  }

  auto CheckTable = [&](const DatabaseTable& a) {
    if (!a.IsExist(sess)) a.CreateTable(sess);
    else if (!a.CheckTable(sess)) throw std::logic_error("");
  };
  try {
    CheckTable(kProbSettingTable);
    CheckTable(kProbExtraAttrTable);
    CheckTable(kTestdataTable);
    CheckTable(kRangeTable);
    CheckTable(kRangeMappingTable);
    CheckTable(kSubmissionTable);
    CheckTable(kTestdataResultTable);
  } catch (const Error& err) {
    cout << "Unexpected error\n" << err << '\n';
    exit(1);
  } catch (const std::logic_error& err) {
    cout << "Table schema check failed.\n";
    exit(1);
  }
}
