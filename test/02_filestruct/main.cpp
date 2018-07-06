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
  }
  virtual void TearDown() {
    sess.sql("DROP DATABASE " + kDatabaseName + ";").execute();
  }
};

TEST(GetProblemTimestampTest, Main) {
  // now problem_settings are empty
  EXPECT_THROW(GetProblemTimestamp(sess, 1001), std::invalid_argument);
}

} // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new Environment);
  return RUN_ALL_TESTS();
}
