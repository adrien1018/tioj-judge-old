#include "filestruct.h"

#include "config.h"

// MySQL table schema
const DatabaseTable kProbSettingTable("problem_settings",
    { //*: used by both stages
      {"problem_id",               "INT",   "UNIQUE NOT NULL"},        //*
      {"check_code_lang",          "INT",   "NOT NULL DEFAULT 0"},
      {"custom_code_lang",         "BOOL",  "NOT NULL DEFAULT FALSE"},
      {"execution_type",           "INT",   "NOT NULL DEFAULT 1"},
      {"file_per_testdata",        "INT",   "NOT NULL DEFAULT 2"},
      {"testdata_file_in_sandbox", "BOOL",  "NOT NULL DEFAULT FALSE"},
      {"pipe_in",                  "INT",   "NOT NULL DEFAULT FALSE"},
      {"pipe_out",                 "INT",   "NOT NULL DEFAULT FALSE"},
      {"partial_judge",            "BOOL",  "NOT NULL DEFAULT TRUE"},
      {"evaluation_type",          "INT",   "NOT NULL"}, //default?
      {"custom_evaluation",        "BOOL",  "NOT NULL DEFAULT FALSE"},
      {"evaluate_nonnormal",       "BOOL",  "NOT NULL DEFAULT FALSE"},
      {"scoring_type",             "INT",   "NOT NULL DEFAULT 1"},     //*
      {"common_file",              "BOOL",  "NOT NULL DEFAULT FALSE"}, //*
      {"custom_stage",             "BOOL",  "NOT NULL DEFAULT FALSE"}, //*
      {"kill_old",                 "BOOL",  "NOT NULL DEFAULT FALSE"}, //1-stage only
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

ProblemSettings::CompileSettings::CompileSettings() :
    lang(kLangNull), args() {}

// Default problem settings
ProblemSettings::ProblemSettings() :
    problem_id(0), check_code_lang(kLangNull), // no code checking
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
    timestamp(0) {}

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
  } catch (const Error& err) {
    cout << "Unexpected error\n" << err << '\n';
    exit(1);
  } catch (const std::logic_error& err) {
    cout << "Table schema check failed.\n";
    exit(1);
  }
}
