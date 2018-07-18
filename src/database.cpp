#include "database.h"

#include <sstream>
#include <iomanip>
#include <iostream>

typedef std::vector<std::string> StrVec;

// ----- MySQL table schema -----

const DatabaseTable kProbSettingTable("problem_settings",
    { //*: used by both stages
      {"problem_id",         "INT",    "UNIQUE NOT NULL"}, //*
      {"is_one_stage",       "BOOL",   "NOT NULL"},        //*
      {"check_code",         "BOOL",   "NOT NULL"},
      {"execution_type",     "INT",    "NOT NULL"},
      {"pipe_in",            "BOOL",   "NOT NULL"},
      {"pipe_out",           "BOOL",   "NOT NULL"},
      {"partial_judge",      "BOOL",   "NOT NULL"},
      {"evaluation_type",    "INT",    "NOT NULL"},
      {"evaluation_format",  "INT",    "NOT NULL"},
      {"password",           "BIGINT", "NOT NULL"},
      {"evaluate_nonnormal", "BOOL",   "NOT NULL"},
      {"scoring_type",       "INT",    "NOT NULL"},        //*
      {"file_per_testdata",  "INT",    "NOT NULL"},
      {"kill_old",           "BOOL",   "NOT NULL"},        //1-stage only
      {"timestamp",          "BIGINT", "NOT NULL"},
    }, {}, "PRIMARY KEY (problem_id)");
const DatabaseTable kProbExtraAttrTable("problem_extra_attr",
    {
      {"problem_id", "INT", "NOT NULL"},
      {"type_id",    "INT", "NOT NULL"},
      {"item_id",    "INT", "NOT NULL"},
      {"text",       "VARCHAR(8192)", "NOT NULL"}
    }, {{"problem_id"}},
    "PRIMARY KEY (problem_id, type_id, item_id), "
    "FOREIGN KEY (problem_id) REFERENCES problem_settings(problem_id)");
const DatabaseTable kTestdataTable("testdata",
    {
      {"problem_id",  "INT",           "NOT NULL"},
      {"testdata_id", "INT",           "NOT NULL"},
      {"time_lim",    "BIGINT",        "NOT NULL"},
      {"memory_lim",  "BIGINT",        "NOT NULL"},
      {"files",       "VARCHAR(1024)", "NOT NULL"}, // file id
      {"args",        "VARCHAR(8192)", "NOT NULL"},
    }, {{"problem_id"}, {"problem_id", "testdata_id"}},
    "PRIMARY KEY (problem_id, testdata_id), "
    "FOREIGN KEY (problem_id) REFERENCES problem_settings(problem_id)");
const DatabaseTable kFileTimestampTable("file_timestamp",
    {
      {"problem_id",  "INT",    "NOT NULL"},
      {"file_id",     "INT",    "NOT NULL"},
      {"timestamp",   "BIGINT", "NOT NULL"}
    }, {{"problem_id"}},
    "PRIMARY KEY (problem_id, file_id), "
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
      {"range_id",    "INT", "NOT NULL"},
    }, {{"problem_id"}},
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

// ----- General -----

// QUESTION: Should sess be global?
void InitMySQLSession(MySQLSession& sess, bool check) {
  using namespace mysqlx;
  using std::cout;
  if (!IsValidDBName(kDatabaseName)) {
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
    else if (check && !a.CheckTable(sess)) throw std::logic_error("");
  };
  try {
    CheckTable(kProbSettingTable);
    CheckTable(kProbExtraAttrTable);
    CheckTable(kTestdataTable);
    CheckTable(kFileTimestampTable);
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

long long GetProblemTimestamp(MySQLSession& sess, int id) {
  auto res = sess.sql("SELECT timestamp FROM problem_settings WHERE problem_id = ?;")
                  .bind(id).execute();
  if (res.count() == 0) {
    throw std::invalid_argument("Problem not exist");
  }
  return static_cast<int64_t>(res.fetchOne()[0]);
}

bool IsJudgeByProblem(MySQLSession& sess, int id) {
  auto res = sess.sql("SELECT is_one_stage, kill_old FROM problem_settings "
                      "WHERE problem_id = ?;").bind(id).execute();
  if (res.count() == 0) {
    throw std::invalid_argument("Problem not exist");
  }
  auto row = res.fetchOne();
  return static_cast<bool>(row[0]) && static_cast<bool>(row[1]);
}

struct AttrEntry {
  enum AttrType {
    kCodeCheckOption = 1,
    kCustomLangSettings = 2,
    kExecSettings = 3,
    kEvalOption = 4,
    kEvalColumn = 5,
    kScoringOption = 6,
    kScoringColumn = 7,
    kTestdataFile = 8,
    kCommonFile = 9,
    kCustomStage = 10
  } type;
  int item_id;
  std::string text;
  AttrEntry() {}
  AttrEntry(AttrType t, int id, const std::string& tx) :
      type(t), item_id(id), text(tx) {}
  AttrEntry(AttrType t, int id, std::string&& tx) :
      type(t), item_id(id), text(std::move(tx)) {}
};

template <class Iter>
ProblemSettings::CompileSettings ParseCompileSettings(Iter first, Iter last) {
  ProblemSettings::CompileSettings s;
  s.lang = static_cast<Language>(std::stoi(*first++));
  s.args = Arguments(++first, last);
  return s;
}

ProblemSettings::ResultColumn ParseColumn(const StrVec& vec) {
  ProblemSettings::ResultColumn s;
  s.type = static_cast<ProblemSettings::ResultColumn::ColumnType>(
      std::stoi(vec[0]));
  s.visible = vec[1] == "1";
  return s;
}

ProblemSettings GetProblemSettings(MySQLSession& sess, int id) {
  auto raw = sess.sql("SELECT * FROM problem_settings WHERE problem_id = ?;")
                 .bind(id).execute();
  if (raw.count() == 0) {
    throw std::invalid_argument("Problem not exist");
  }
  if (raw.count() > 1) {
    std::cerr << "Duplicate problem entry?\n";
    exit(1);
  }
  ProblemSettings ps;
  auto res = raw.fetchOne();
  auto Get = [&res](int z) { return static_cast<int>(res[z]); };

  ps.problem_id = Get(0);
  ps.is_one_stage = Get(1);
  // ProblemSettings::CompileSettings::lang is null by default
  // if (!Get(2)) ps.check_code_compile.lang = kLangNull;
  ps.execution_type = static_cast<ProblemSettings::ExecutionType>(Get(3));
  ps.pipe_in = Get(4);
  ps.pipe_out = Get(5);
  ps.partial_judge = Get(6);
  ps.evaluation_type = static_cast<ProblemSettings::EvaluationType>(Get(7));
  ps.evaluation_format = static_cast<ProblemSettings::EvalOutputFormat>(Get(8));
  ps.password = static_cast<int64_t>(res[9]);
  ps.evaluate_nonnormal = Get(10);
  ps.scoring_type = static_cast<ProblemSettings::ScoringType>(Get(11));
  ps.file_per_testdata = Get(12);
  ps.kill_old = Get(13);
  ps.timestamp = static_cast<int64_t>(Get(14));

  raw = sess.sql("SELECT * FROM problem_extra_attr WHERE problem_id = ? "
                 "ORDER BY type_id, item_id").bind(id).execute();
  for (; raw.count() != 0;) {
    res = raw.fetchOne();
    AttrEntry::AttrType type = static_cast<AttrEntry::AttrType>(Get(1));
    int item_id = Get(2);
    std::string str = res[3];
    StrVec cont = SplitString(str);
    switch (type) {
      case AttrEntry::kCodeCheckOption: {
        ps.code_check_compile = ParseCompileSettings(cont.begin(), cont.end());
        break;
      }
      case AttrEntry::kCustomLangSettings: {
        ProblemSettings::CustomLanguage s;
        s.as_interpreter = cont[0] == "1";
        s.tl_a = std::stod(cont[1]);
        s.tl_b = std::stod(cont[2]);
        s.ml_a = std::stod(cont[3]);
        s.ml_b = std::stod(cont[4]);
        size_t sysadjcnt = std::stoi(cont[5]);
        for (size_t i = 0; i < sysadjcnt; i++) {
          auto& cur = cont[i + 6];
          s.syscall_adj.emplace_back(cur.substr(1), cur[0] == '-');
        }
        s.compile = ParseCompileSettings(cont.begin() + (sysadjcnt + 6),
            cont.end());
        ps.custom_lang.push_back(std::move(s));
        break;
      }
      case AttrEntry::kExecSettings: {
        switch (ps.execution_type) {
          case ProblemSettings::kExecNormal: break;
          case ProblemSettings::kExecOldInteractive: // fall through
          case ProblemSettings::kExecInteractive:    // fall through
          case ProblemSettings::kExecMultiPhaseSeparated: {
            ps.execution_times = std::stoi(str);
            break;
          }
          case ProblemSettings::kExecCFInteractive: // fall through
          case ProblemSettings::kExecOutputOnly: {
            ps.lib_compile = ParseCompileSettings(cont.begin(), cont.end());
            break;
          }
        }
        break;
      }
      case AttrEntry::kEvalOption: {
        ps.evaluation_compile = ParseCompileSettings(cont.begin(), cont.end());
        break;
      }
      case AttrEntry::kScoringOption: {
        ps.scoring_compile = ParseCompileSettings(cont.begin(), cont.end());
        break;
      }
      case AttrEntry::kEvalColumn: {
        ps.evaluation_columns.push_back(ParseColumn(cont));
        break;
      }
      case AttrEntry::kScoringColumn: {
        ps.scoring_columns.push_back(ParseColumn(cont));
        break;
      }
      case AttrEntry::kTestdataFile: {
        ProblemSettings::TestdataFile f;
        f.id = item_id;
        f.path = str;
        ps.testdata_files.push_back(std::move(f));
        break;
      }
      case AttrEntry::kCommonFile: {
        ProblemSettings::CommonFile f;
        f.usage = static_cast<decltype(f.usage)>(std::stoi(cont[0]));
        f.lib_lang = static_cast<Language>(std::stoi(cont[1]));
        f.id = std::stoi(cont[2]);
        int tmp = std::stoi(cont[3]);
        for (int i = 0; i < 4; i++) f.stages[i] = (tmp & 1 << i);
        f.file_id = std::stoi(cont[4]);
        f.path = cont[5];
        ps.common_files.push_back(std::move(f));
        break;
      }
      case AttrEntry::kCustomStage: {
        ps.custom_stage.emplace_back(item_id,
            ParseCompileSettings(cont.begin(), cont.end()));
        break;
      }
    }
  }
  raw = sess.sql("SELECT * FROM testdata WHERE problem_id = ? "
                 "ORDER BY testdata_id").bind(id).execute();
  for (; raw.count() != 0;) {
    res = raw.fetchOne();
    ProblemSettings::Testdata t;
    t.time_lim = static_cast<int64_t>(res[2]);
    t.memory_lim = static_cast<int64_t>(res[3]);
    StrVec ids = SplitString(res[4]);
    for (const std::string& str : ids) t.file_id.push_back(std::stoi(str));
    t.args = SplitString(res[5]);
    ps.testdata.push_back(std::move(t));
  }
  raw = sess.sql("SELECT * FROM ranges WHERE problem_id = ? "
                 "ORDER BY range_id").bind(id).execute();
  ps.ranges.resize(raw.count());
  for (int i = 0; raw.count() != 0; i++) {
    res = raw.fetchOne();
    ps.ranges[i].score = static_cast<int64_t>(res[2]);
  }
  raw = sess.sql("SELECT * FROM range_mapping WHERE problem_id = ? "
                 "ORDER BY testdata_id").bind(id).execute();
  for (; raw.count() != 0;) {
    res = raw.fetchOne();
    ps.ranges[static_cast<int>(res[2])].testdata
      .push_back(static_cast<int>(res[1]));
  }

  return ps;
}

bool IsProbExist(MySQLSession& sess, int id) {
  return static_cast<int>(
      sess.sql("SELECT EXISTS(SELECT 1 FROM problem_settings WHERE problem_id = ?);")
          .bind(id).execute().fetchOne()[0]);
}

auto DoubleToStr = [](double a) {
  std::stringstream ss;
  ss << std::setprecision(16) << a;
  return ss.str();
};

std::string CompileSettingsToStr(const ProblemSettings::CompileSettings& s) {
  return MergeString(
      MergeString(std::to_string(s.lang), std::to_string(s.args.size())), // langid, argc
      MergeString(s.args)); // argv
}

std::string CustomLanguageToStr(const ProblemSettings::CustomLanguage& s) {
  return MergeString(MergeString(MergeString(
      std::string(s.as_interpreter ? "1" : "0"),
      MergeString(std::vector<double>({ // TL / ML adjustments
        s.tl_a, s.tl_b, s.ml_a, s.ml_b}), DoubleToStr)),
      std::to_string(s.syscall_adj.size())), MergeString( // syscall adjustments
      MergeString(s.syscall_adj, [](const std::pair<std::string, bool>& a) {
        return (a.second ? "-" : "+") + a.first;
      }),
      CompileSettingsToStr(s.compile))); // compile settings
}

std::string CommonFileToStr(const ProblemSettings::CommonFile& s) {
  StrVec vec;
  vec.push_back(std::to_string(static_cast<int>(s.usage)));
  vec.push_back(std::to_string(static_cast<int>(s.lib_lang)));
  vec.push_back(std::to_string(s.id));
  int tmp = 0;
  for (int i = 0; i < 4; i++) {
    if (s.stages[i]) tmp |= 1 << i;
  }
  vec.push_back(std::to_string(tmp));
  vec.push_back(std::to_string(s.file_id));
  vec.push_back(s.path);
  return MergeString(vec);
}

std::string ColumnToStr(const ProblemSettings::ResultColumn& s) {
  return MergeString(std::to_string(static_cast<int>(s.type)),
                     std::string(s.visible ? "1" : "0"));
}

void UpdateProblemSettings(MySQLSession& sess, const ProblemSettings& ps) {
  std::vector<AttrEntry> opts;
  // CodeCheckOption
  if (ps.code_check_compile.lang != kLangNull) {
    opts.emplace_back(AttrEntry::kCodeCheckOption, 0,
        CompileSettingsToStr(ps.code_check_compile));
  }
  // CustomLangSettings
  for (size_t i = 0; i < ps.custom_lang.size(); i++) {
    opts.emplace_back(AttrEntry::kCustomLangSettings, i,
        CustomLanguageToStr(ps.custom_lang[i]));
  }
  // ExecSettings
  switch (ps.execution_type) {
    case ProblemSettings::kExecNormal: break;
    case ProblemSettings::kExecOldInteractive: // fall through
    case ProblemSettings::kExecInteractive:    // fall through
    case ProblemSettings::kExecMultiPhaseSeparated: {
      opts.emplace_back(AttrEntry::kExecSettings, 0,
          std::to_string(ps.execution_times));
      break;
    }
    case ProblemSettings::kExecCFInteractive: // fall through
    case ProblemSettings::kExecOutputOnly: {
      opts.emplace_back(AttrEntry::kExecSettings, 0,
          CompileSettingsToStr(ps.lib_compile));
      break;
    }
  }
  // EvalOption, ScoringOption
  if (ps.evaluation_type != ProblemSettings::kEvalNormal &&
      ps.evaluation_type != ProblemSettings::kEvalSkip) {
    opts.emplace_back(AttrEntry::kEvalOption, 0,
        CompileSettingsToStr(ps.evaluation_compile));
  }
  if (ps.scoring_type != ProblemSettings::kScoringNormal ||
      ps.is_one_stage) {
    opts.emplace_back(AttrEntry::kScoringOption, 0,
        CompileSettingsToStr(ps.scoring_compile));
  }
  // EvalColumn, ScoringColumn
  for (size_t i = 0; i < ps.evaluation_columns.size(); i++) {
    opts.emplace_back(AttrEntry::kEvalColumn, i,
        ColumnToStr(ps.evaluation_columns[i]));
  }
  for (size_t i = 0; i < ps.scoring_columns.size(); i++) {
    opts.emplace_back(AttrEntry::kScoringColumn, i,
        ColumnToStr(ps.scoring_columns[i]));
  }
  // TestdataFile, CommonFile
  for (const ProblemSettings::TestdataFile& f : ps.testdata_files) {
    opts.emplace_back(AttrEntry::kTestdataFile, f.id, f.path);
  }
  for (size_t i = 0; i < ps.common_files.size(); i++) {
    opts.emplace_back(AttrEntry::kCommonFile, i,
        CommonFileToStr(ps.common_files[i]));
  }
  // CustomStage
  for (const auto& p : ps.custom_stage) {
    opts.emplace_back(AttrEntry::kCustomStage, p.first,
        CompileSettingsToStr(p.second));
  }

  // SqlStatement have no default constuctor...
  mysqlx::SqlStatement query(sess.sql(""));
  if (IsProbExist(sess, ps.problem_id)) {
    // delete additional attr and range mapping first because of foreign keys
    sess.sql("DELETE FROM range_mapping WHERE problem_id = ?;")
        .bind(ps.problem_id).execute();
    sess.sql("DELETE FROM problem_extra_attr WHERE problem_id = ?;")
        .bind(ps.problem_id).execute();
    sess.sql("DELETE FROM testdata WHERE problem_id = ?;")
        .bind(ps.problem_id).execute();
    sess.sql("DELETE FROM ranges WHERE problem_id = ?;")
        .bind(ps.problem_id).execute();
    query = sess.sql("UPDATE problem_settings SET "
        "is_one_stage = ?, check_code = ?, execution_type = ?, "
        "pipe_in = ?, pipe_out = ?, partial_judge = ?, evaluation_type = ?, "
        "evaluation_format = ?, password = ?, evaluate_nonnormal = ?, "
        "scoring_type = ?, file_per_testdata = ?, "
        "kill_old = ?, timestamp = ? WHERE problem_id = ?;");
  } else {
    query = sess.sql("INSERT INTO problem_settings ("
        "is_one_stage, check_code, execution_type, pipe_in, pipe_out, "
        "partial_judge, evaluation_type, evaluation_format, password, "
        "evaluate_nonnormal, scoring_type, file_per_testdata, "
        "kill_old, timestamp, problem_id) VALUES "
        "(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
  }
  query.bind(static_cast<int>(ps.is_one_stage),
             static_cast<int>(ps.code_check_compile.lang != kLangNull),
             static_cast<int>(ps.execution_type),
             static_cast<int>(ps.pipe_in),
             static_cast<int>(ps.pipe_out),
             static_cast<int>(ps.partial_judge),
             static_cast<int>(ps.evaluation_type),
             static_cast<int>(ps.evaluation_format),
             static_cast<int64_t>(ps.password),
             static_cast<int>(ps.evaluate_nonnormal),
             static_cast<int>(ps.scoring_type),
             ps.file_per_testdata,
             static_cast<int>(ps.kill_old),
             static_cast<int64_t>(ps.timestamp),
             ps.problem_id).execute();
  for (const AttrEntry& attr : opts) {
    // prob-id, type, item, text
    sess.sql("INSERT INTO problem_extra_attr VALUES (?, ?, ?, ?);")
        .bind(ps.problem_id, static_cast<int>(attr.type),
              attr.item_id, attr.text).execute();
  }

  for (size_t i = 0; i < ps.testdata.size(); i++) {
    // prob-id, testdata-id, TL, ML, file-ids, args
    sess.sql("INSERT INTO testdata VALUES (?, ?, ?, ?, ?, ?);")
        .bind(ps.problem_id, i, ps.testdata[i].time_lim,
              ps.testdata[i].memory_lim,
              MergeString(ps.testdata[i].file_id,
                  [](int i){return std::to_string(i);}),
              MergeString(ps.testdata[i].args)).execute();
  }
  for (size_t i = 0; i < ps.ranges.size(); i++) {
    // prob-id, range-id, score
    sess.sql("INSERT INTO ranges VALUES (?, ?, ?);")
        .bind(ps.problem_id, i, static_cast<int64_t>(ps.ranges[i].score))
        .execute();
    for (int testdata : ps.ranges[i].testdata) {
      // prob-id, testdata, range
      sess.sql("INSERT INTO range_mapping VALUES (?, ?, ?);")
          .bind(ps.problem_id, testdata, i).execute();
    }
  }
}

