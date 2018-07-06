#include <regex>
#include <string>

#include <gtest/gtest.h>

#include "filestruct.h"

const std::string kDatabaseName = "tioj_judge_test";
const std::string kMySQLPassword = "tioj_test";
const std::string kMySQLHostname = "localhost";
const int kMySQLPort = 33060;
const std::string kMySQLUsername = "root";

MySQLSession sess;

namespace {

class Environment : public ::testing::Environment {
 public:
  virtual void SetUp() {
    InitMySQLSession(sess);
#ifdef OUTPUT_QUERY
    sess.sql("DELETE FROM mysql.general_log;").execute();
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

TEST(GetProblemTimestampTest, Main) {
  // now problem_settings are empty
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

} // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new Environment);
  return RUN_ALL_TESTS();
}
