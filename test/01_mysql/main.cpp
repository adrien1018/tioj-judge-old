#include <regex>
#include <string>

#include <gtest/gtest.h>

#include "mysql_helper.h"

const std::string kTestDB = "tioj_judge_test";
const std::string kTestDB2 = "tioj_judge_test2";
const std::string kPasswd = "tioj_test";

namespace {

class MySQLSessionTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    sess.Start("localhost", 33060, "root", kPasswd);
    sess.ChangeDatabase(kTestDB2, true);
    sess.ChangeDatabase(kTestDB, true);
  }
  virtual void TearDown() {
    sess.sql("DROP DATABASE `" + kTestDB + "`;").execute();
    sess.sql("DROP DATABASE `" + kTestDB2 + "`;").execute();
  }

  MySQLSession sess;
};

TEST(MySQLSessionBasic, FailedConnection) {
  MySQLSession sess;
  EXPECT_THROW(sess.Start("localhost", 33060, "root", "notpassword"),
      mysqlx::Error);
  EXPECT_THROW(sess.Start("localhost", 3306, "root", kPasswd),
      mysqlx::Error);
}

TEST(MySQLSessionBasic, Connection) {
  MySQLSession sess, sess2;
  ASSERT_NO_THROW(sess.Start("localhost", 33060, "root", kPasswd));
  EXPECT_EQ(1, sess.sql("SELECT 2*5;").execute().count());
  EXPECT_EQ(10, (int)(*sess.sql("SELECT 2*5;").execute().fetchAll().begin())[0]);
  EXPECT_EQ(10, (int)sess.sql("SELECT 2*5;").execute().fetchOne()[0]);
  int conn_id = sess.sql("SELECT connection_id();").execute().fetchOne()[0];
  sess2 = std::move(sess);
  EXPECT_THROW(sess.sql("SELECT 2*5;").execute(), std::logic_error);
  EXPECT_EQ(conn_id, (int)sess2.sql("SELECT connection_id();").execute().fetchOne()[0]);
  EXPECT_NO_THROW(sess2.Stop());
  EXPECT_THROW(sess2.sql("SELECT 2*5;").execute(), std::logic_error);
  EXPECT_NO_THROW(sess.Start("localhost", 33060, "root", kPasswd));
  EXPECT_NO_THROW(sess2.Start("localhost", 33060, "root", kPasswd));
  EXPECT_NO_THROW(sess.Stop());
  EXPECT_NO_THROW(sess2.Stop());
}

TEST_F(MySQLSessionTest, ChangeDatabase) {
  EXPECT_EQ(0, sess.sql("SHOW TABLES;").execute().count());
  EXPECT_NO_THROW(sess.sql("CREATE TABLE test ( id int unique );").execute());
  EXPECT_EQ(1, sess.sql("SHOW TABLES;").execute().count());
  EXPECT_NO_THROW(sess.ChangeDatabase(kTestDB2, false));
  EXPECT_THROW(sess.ChangeDatabase("DB_NOT_EXIST", false), mysqlx::Error);
  EXPECT_EQ(0, sess.sql("SHOW TABLES;").execute().count());
}

TEST_F(MySQLSessionTest, StringFormat) {
  std::string table = "test", col = "id";
  int val = 213;
  EXPECT_NO_THROW(sess.sql("CREATE TABLE %s ( %s int unique );", table, col.c_str()).execute());
  EXPECT_NO_THROW(sess.sql("INSERT INTO %s (%s) VALUES (%d);", table, col, val).execute());
  EXPECT_EQ(1, sess.sql("SELECT * FROM %s;", table).execute().count());
  EXPECT_EQ(213, (int)sess.sql("SELECT * FROM %s;", table).execute().fetchOne()[0]);
}

class DatabaseTableTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    sess.Start("localhost", 33060, "root", kPasswd);
    sess.ChangeDatabase(kTestDB, true);
  }
  virtual void TearDown() {
    sess.sql("DROP DATABASE `" + kTestDB + "`;").execute();
  }
  MySQLSession sess;
};

TEST(DatabaseTableBasic, SchemaChecking) {
  EXPECT_THROW(DatabaseTable("test", {}, {}, ""),
      std::invalid_argument);
  EXPECT_THROW(DatabaseTable("tes-t", {{"id", "int", "unique"}}, {}, ""),
      std::invalid_argument);
  EXPECT_THROW(DatabaseTable("test", {{"i-d", "int", "unique"}}, {}, ""),
      std::invalid_argument);
  EXPECT_THROW(DatabaseTable("test", {{"", "int", "unique"}}, {}, ""),
      std::invalid_argument);
  EXPECT_THROW(DatabaseTable("test", {{"id", "int", "unique"}}, {{"td"}}, ""),
      std::invalid_argument);
  EXPECT_THROW(DatabaseTable("test", {{"id", "int", "unique"}, {"td", "int", ""}},
      {{"td"}, {"id"}, {"td", "sd"}}, ""), std::invalid_argument);
  EXPECT_NO_THROW(DatabaseTable("test", {{"id", "int", "unique"}}, {}, ""));
  EXPECT_NO_THROW(DatabaseTable("test", {{"id", "int", ""}}, {{"id"}}, ""));
  EXPECT_NO_THROW(DatabaseTable("test", {{"id", "int", "unique"}, {"td", "int", ""}},
      {{"td"}, {"id"}, {"td", "id"}}, ""));
}

TEST(DatabaseTableBasic, Indexing) {
  DatabaseTable schema("test", {
        {"id", "int", "unique"},
        {"a",  "int", ""},
        {"b",  "varchar(16)", "not null"}
      }, {{"id", "a"}}, "");
  EXPECT_EQ(2, schema["b"]);
  EXPECT_EQ(0, schema["id"]);
  EXPECT_THROW(schema["c"], std::out_of_range);
  EXPECT_EQ("id", schema[0]);
  EXPECT_THROW(schema[3], std::out_of_range);
}

TEST_F(DatabaseTableTest, CreateTable) {
  DatabaseTable schema("test", {
        {"id", "int", "unique"},
        {"a",  "int", ""},
        {"b",  "varchar(16)", "not null"}
      }, {{"id", "a"}}, "");
  EXPECT_NO_THROW(schema.CreateTable(sess));
  std::vector<mysqlx::Row> res =
    sess.sql("SHOW COLUMNS FROM test;").execute().fetchAll();
  EXPECT_EQ(3, res.size());
  EXPECT_EQ("id", (std::string)res[0][0]);
  EXPECT_EQ("a", (std::string)res[1][0]);
  EXPECT_EQ("b", (std::string)res[2][0]);
  EXPECT_EQ("int(11)", (std::string)res[0][1]);
  EXPECT_EQ("varchar(16)", (std::string)res[2][1]);
  EXPECT_EQ("YES", (std::string)res[0][2]);
  EXPECT_EQ("NO", (std::string)res[2][2]);
  EXPECT_EQ("UNI", (std::string)res[0][3]);
  EXPECT_EQ("", (std::string)res[1][3]);

  std::string schema_str = sess.sql("SHOW CREATE TABLE test;").execute().fetchOne()[1];
  EXPECT_TRUE(std::regex_search(schema_str, std::regex("UNIQUE KEY .* \\(`id`\\)")));
  EXPECT_TRUE(std::regex_search(schema_str, std::regex("KEY .* \\(`id`,`a`\\)")));
}

TEST_F(DatabaseTableTest, CheckTable) {
  DatabaseTable schema("test", {
        {"id", "int", "unique"},
        {"a",  "int", ""},
        {"b",  "varchar(16)", "not null"}
      }, {{"id", "a"}}, "");
  EXPECT_FALSE(schema.IsExist(sess));
  EXPECT_FALSE(schema.CheckTable(sess));
  EXPECT_NO_THROW(schema.CreateTable(sess));
  EXPECT_TRUE(schema.IsExist(sess));
  EXPECT_TRUE(schema.CheckTable(sess));

  std::string schema_str = sess.sql("SHOW CREATE TABLE test;").execute().fetchOne()[1];
  std::smatch mt;
  std::regex_search(schema_str, mt, std::regex("KEY `(.*)` \\(`id`,`a`\\)"));
  std::string index_name = mt[1];
  sess.sql("ALTER TABLE test DROP INDEX %s;", index_name).execute();
  EXPECT_TRUE(schema.IsExist(sess));
  EXPECT_FALSE(schema.CheckTable(sess));

  sess.sql("CREATE INDEX test_id_a ON test (id,a);", index_name).execute();
  EXPECT_TRUE(schema.IsExist(sess));
  EXPECT_TRUE(schema.CheckTable(sess));

  sess.sql("CREATE TABLE acpxzenkgt (id int);").execute();
  EXPECT_TRUE(schema.CheckTable(sess)); // will use another temp table
}

} // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
