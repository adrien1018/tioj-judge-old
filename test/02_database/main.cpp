#include <regex>
#include <string>

#include <gtest/gtest.h>

#include "database.h"

const std::string kDatabaseName = "tioj_judge_test";
const std::string kMySQLPassword = "tioj_test";
const std::string kMySQLHostname = "localhost";
const int kMySQLPort = 33060;
const std::string kMySQLUsername = "root";

MySQLSession sess;

#define SAME(X) bool operator==(const X& a, const X& b)
#define CHK(x) (!(a.x == b.x) ? std::cerr << #x << std::endl, false : true)
SAME(ScoreInt) {
  return static_cast<int64_t>(a) == static_cast<int64_t>(b);
}
SAME(ProblemSettings::CompileSettings) {
  return CHK(lang) && CHK(args);
}
SAME(ProblemSettings::CustomLanguage) {
  return CHK(compile) && CHK(as_interpreter) && CHK(tl_a) && CHK(tl_b) &&
         CHK(ml_a) && CHK(ml_b) && CHK(syscall_adj);
}
SAME(ProblemSettings::ResultColumn) {
  return CHK(type) && CHK(visible);
}
SAME(ProblemSettings::TestdataFile) {
  return CHK(id) && CHK(path);
}
SAME(ProblemSettings::CommonFile) {
  return CHK(usage) && CHK(lib_lang) && CHK(id) && CHK(stages[0]) &&
         CHK(stages[1]) && CHK(stages[2]) && CHK(stages[3]) && CHK(file_id) &&
         CHK(path);
}
SAME(ProblemSettings::Testdata) {
  return CHK(time_lim) && CHK(memory_lim) && CHK(args) && CHK(file_id);
}
SAME(ProblemSettings::ScoreRange) {
  return CHK(score) && CHK(testdata);
}
SAME(ProblemSettings) {
  return CHK(problem_id) && CHK(is_one_stage) && CHK(code_check_compile) &&
         CHK(custom_lang) && CHK(execution_type) && CHK(execution_times) &&
         CHK(lib_compile) && CHK(pipe_in) && CHK(pipe_out) &&
         CHK(partial_judge) && CHK(evaluation_type) && CHK(evaluation_format) &&
         CHK(password) && CHK(evaluation_compile) && CHK(evaluation_columns) &&
         CHK(evaluate_nonnormal) && CHK(scoring_type) && CHK(scoring_compile) &&
         CHK(scoring_columns) && CHK(file_per_testdata) &&
         CHK(testdata_files) && CHK(common_files) && CHK(kill_old) &&
         CHK(custom_stage) && CHK(testdata) && CHK(ranges) && CHK(timestamp);
}

namespace {

class Environment : public ::testing::Environment {
 public:
  virtual void SetUp() {
    InitMySQLSession(sess);
#ifdef OUTPUT_QUERY
    sess.sql("SET GLOBAL general_log = 'OFF';").execute();
    sess.sql("TRUNCATE TABLE mysql.general_log;").execute();
    sess.sql("SET GLOBAL log_output = 'TABLE';").execute();
    sess.sql("SET GLOBAL general_log = 'ON';").execute();
#endif
  }
  virtual void TearDown() {
    sess.sql("DROP DATABASE " + kDatabaseName + ";").execute();
#ifdef OUTPUT_QUERY
    auto result = sess.sql("SELECT argument FROM mysql.general_log;").execute();
    std::cout << result.count() << std::endl;
    for (auto i : result) {
      std::cout << (std::string)i[0] << std::endl;
    }
#endif
  }
};

TEST(UpdateProblemSettingsTest, MainProbSettings) {
  ProblemSettings prob;
  prob.problem_id = 2001;
  prob.timestamp = 12345678901;
  UpdateProblemSettings(sess, prob);
  EXPECT_EQ(12345678901,
      (int64_t)sess.sql("SELECT timestamp from problem_settings;")
                   .execute().fetchOne()[0]);
  EXPECT_EQ(1, (int)sess.sql("SELECT * from problem_settings;").execute().count());
  prob.timestamp = 12345678902;
  UpdateProblemSettings(sess, prob);
  EXPECT_EQ(12345678902,
      (int64_t)sess.sql("SELECT timestamp from problem_settings;")
                   .execute().fetchOne()[0]);
  EXPECT_EQ(1, (int)sess.sql("SELECT * from problem_settings;").execute().count());
  prob.problem_id = 2002;
  UpdateProblemSettings(sess, prob);
  EXPECT_EQ(2, (int)sess.sql("SELECT * from problem_settings;").execute().count());
  EXPECT_EQ(0, (int)sess.sql("SELECT * from problem_extra_attr;").execute().count());
  sess.sql("DELETE FROM problem_settings;").execute();
}

bool CheckSame(const ProblemSettings& prob) {
  UpdateProblemSettings(sess, prob);
  ProblemSettings prob2(GetProblemSettings(sess, prob.problem_id));
  return prob == prob2;
}

class ProbSettingsRWTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    comp.lang = kLangCpp;
    comp.args = {"-O2", "-lgmpxx", "-lgmp"};
    comp_def.args = {"--strict", "--float", "--error=1e-6", "--relative"};
    lang.compile = comp;
    lang.tl_b = 100;
    lang.syscall_adj.push_back({"fcntl", true});
    lang.syscall_adj.push_back({"lseek", false});
    cols.resize(3);
    cols[2].type = ProblemSettings::ResultColumn::kScoreFloat;
    cols[2].visible = false;

    ProblemSettings::Testdata test;
    test.file_id = {2, 4};
    test.args = {"--testargs", "-t"};
    prob.testdata.push_back(test);
    test.file_id = {3, 5};
    prob.testdata.push_back(test);
    ProblemSettings::ScoreRange range;
    range.score = 1.0;
    range.testdata = {0};
    prob.ranges.push_back(range);
    range.score = 12.335;
    range.testdata = {0, 1};
    prob.ranges.push_back(range);
  }
  virtual void TearDown() {
    sess.sql("DELETE FROM problem_extra_attr;").execute();
    sess.sql("DELETE FROM range_mapping;").execute();
    sess.sql("DELETE FROM testdata;").execute();
    sess.sql("DELETE FROM ranges;").execute();
    sess.sql("DELETE FROM problem_settings;").execute();
  }

  ProblemSettings prob;
  ProblemSettings::CompileSettings comp, comp_def;
  ProblemSettings::CustomLanguage lang;
  decltype(prob.evaluation_columns) cols;
};

#define CHECK EXPECT_TRUE(CheckSame(prob))

TEST_F(ProbSettingsRWTest, Basic) {
  CHECK;
  prob.problem_id = 1234;
  CHECK;
}

TEST_F(ProbSettingsRWTest, OneStage) {
  prob.is_one_stage = true;
  prob.scoring_compile = comp;
  CHECK;
  prob.kill_old = true;
  CHECK;
}

TEST_F(ProbSettingsRWTest, CodeCheckCompile) {
  CHECK;
  prob.code_check_compile = comp;
  CHECK;
  CHECK;
}

TEST_F(ProbSettingsRWTest, CustomLang) {
  prob.custom_lang.push_back(lang);
  lang.tl_b = 0; lang.syscall_adj.clear();
  prob.custom_lang.push_back(lang);
  CHECK;
  // TODO: CustomLang with interpreter
}

TEST_F(ProbSettingsRWTest, ExecSettings) {
  ProblemSettings::CommonFile f;
  f.usage = ProblemSettings::CommonFile::kLib;
  f.lib_lang = kLangCpp;
  f.id = 0;
  f.file_id = 15;
  f.path = "lib1234.h";

  prob.execution_type = ProblemSettings::kExecOldInteractive;
  prob.execution_times = 3;
  prob.common_files.push_back(f);
  CHECK;
  prob.execution_type = ProblemSettings::kExecInteractive;
  f.lib_lang = kLangC;
  f.file_id = 19;
  prob.common_files.push_back(f);
  CHECK;
  prob.common_files.clear();

  prob.execution_type = ProblemSettings::kExecMultiPhaseSeparated;
  f.path = "a.h"; f.file_id = 16; f.id = 0;
  prob.common_files.push_back(f);
  f.path = "b.h"; f.file_id = 17; f.id = 1;
  prob.common_files.push_back(f);
  f.path = "c.h"; f.file_id = 33; f.id = 2;
  prob.common_files.push_back(f);
  CHECK;
  prob.common_files.clear();

  prob.execution_type = ProblemSettings::kExecCFInteractive;
  prob.lib_compile = comp;
  prob.execution_times = 1;
  f.path = "lib.cpp"; f.lib_lang = kLangNull; f.id = 0;
  prob.common_files.push_back(f);
  CHECK;
  prob.execution_type = ProblemSettings::kExecOutputOnly;
  CHECK;
}

TEST_F(ProbSettingsRWTest, OtherExec) {
  prob.pipe_in = true;
  CHECK;
  prob.pipe_out = true;
  CHECK;
  prob.partial_judge = true;
  CHECK;
}

TEST_F(ProbSettingsRWTest, Evaluation) {
  prob.evaluation_type = ProblemSettings::kEvalSkip;
  prob.evaluation_format = ProblemSettings::kEvalFormatFloat;
  CHECK;
  prob.evaluation_type = ProblemSettings::kEvalOldSpecialJudge;
  prob.evaluation_compile = comp;
  CHECK;
  prob.evaluation_type = ProblemSettings::kEvalSpecialJudge;
  prob.evaluation_format = ProblemSettings::kEvalFormatZero;
  CHECK;
  prob.evaluation_columns = cols;
  CHECK;
  prob.evaluation_type = ProblemSettings::kEvalDefaultProgram;
  prob.evaluation_columns.clear();
  prob.evaluation_compile = comp_def;
  CHECK;
  prob.evaluate_nonnormal = true;
  CHECK;
}

TEST_F(ProbSettingsRWTest, Scoring) {
  prob.scoring_type = ProblemSettings::kScoringWeighted;
  CHECK;
  prob.scoring_type = ProblemSettings::kScoringSpecial;
  prob.scoring_compile = comp;
  CHECK;
  prob.scoring_columns = cols;
  CHECK;
}

TEST_F(ProbSettingsRWTest, TestdataFile) {
  prob.file_per_testdata = 3;
  CHECK;
  ProblemSettings::TestdataFile f;
  f.id = 0;
  f.path = "test1";
  prob.testdata_files.push_back(f);
  CHECK;
  f.id = 1;
  f.path = "test3";
  prob.testdata_files.push_back(f);
  CHECK;
}

TEST_F(ProbSettingsRWTest, CommonFile) {
  ProblemSettings::CommonFile f;
  f.usage = ProblemSettings::CommonFile::kOther;
  f.stages[2] = true;
  f.file_id = 15; f.path = "g.test";
  prob.common_files.push_back(f);
  CHECK;
  f.stages[3] = true;
  f.file_id = 5; f.path = "abc/abc.test";
  prob.common_files.push_back(f);
  CHECK;
  f.stages[0] = true; f.stages[1] = true; f.stages[2] = false;
  f.file_id = 2; f.path = "test\"test";
  prob.common_files.push_back(f);
  CHECK;
}

TEST_F(ProbSettingsRWTest, CustomStage) {
  ProblemSettings::CommonFile f;
  f.usage = ProblemSettings::CommonFile::kCustomStage;
  f.id = 1; f.file_id = 34;
  prob.common_files.push_back(f);
  prob.custom_stage.emplace_back(1, comp);
  CHECK;
  f.id = 2; f.file_id = 77;
  prob.common_files.push_back(f);
  prob.custom_stage.emplace_back(2, comp);
  CHECK;
}

TEST(GetProblemTimestampTest, Main) {
  EXPECT_THROW(GetProblemTimestamp(sess, 1001), std::invalid_argument);
  ProblemSettings prob;
  prob.problem_id = 1001;
  prob.timestamp = 63191117070;
  UpdateProblemSettings(sess, prob);
  prob.problem_id = 1002;
  prob.timestamp = 63076601597;
  UpdateProblemSettings(sess, prob);
  EXPECT_EQ(63191117070, GetProblemTimestamp(sess, 1001));
  EXPECT_EQ(63076601597, GetProblemTimestamp(sess, 1002));
  sess.sql("DELETE FROM problem_settings;").execute();
}

TEST(JudgeByProblemTest, Main) {
  EXPECT_THROW(IsJudgeByProblem(sess, 1001), std::invalid_argument);
  ProblemSettings prob;
  prob.problem_id = 1001;
  prob.is_one_stage = true;
  UpdateProblemSettings(sess, prob);
  prob.problem_id = 1002;
  prob.kill_old = true;
  UpdateProblemSettings(sess, prob);
  EXPECT_FALSE(IsJudgeByProblem(sess, 1001));
  EXPECT_TRUE(IsJudgeByProblem(sess, 1002));
  prob.is_one_stage = false;
  UpdateProblemSettings(sess, prob);
  EXPECT_FALSE(IsJudgeByProblem(sess, 1002));
  sess.sql("DELETE FROM problem_extra_attr;").execute();
  sess.sql("DELETE FROM problem_settings;").execute();
}

} // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new Environment);
  return RUN_ALL_TESTS();
}
