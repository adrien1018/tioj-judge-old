#include "mysql_helper.h"

#include <cstdio>
#include <random>
#include <regex>

// ----- MySQLSession -----

void MySQLSession::CheckNull_() const {
  if (!sess_) throw std::logic_error("Session not initialized");
}

MySQLSession::MySQLSession() : sess_(nullptr) {}

MySQLSession::MySQLSession(MySQLSession&& rhs) : sess_(rhs.sess_) {
  rhs.sess_ = nullptr;
}

void MySQLSession::operator=(MySQLSession&& rhs) {
  if (sess_) delete sess_;
  sess_ = rhs.sess_;
  rhs.sess_ = nullptr;
}

MySQLSession::~MySQLSession() {
  if (sess_) delete sess_;
}

void MySQLSession::Stop() {
  if (sess_) {
    delete sess_;
    sess_ = nullptr;
  }
}

void MySQLSession::ChangeDatabase(const std::string& name, bool create) {
  if (create) {
    try {
      sess_->sql("USE `" + name + "`;").execute();
    } catch (const mysqlx::Error&) {
      sess_->sql("CREATE DATABASE `" + name + "`;").execute();
      sess_->sql("USE `" + name + "`;").execute();
    }
  } else {
    sess_->sql("USE `" + name + "`;").execute();
  }
}

mysqlx::SqlStatement MySQLSession::sql(const std::string& query) {
  CheckNull_();
  _DEBUG2("query", query);
  return sess_->sql(query);
}

// ----- DatabaseTable -----

typedef std::initializer_list<std::string> ILStr_;
typedef std::initializer_list<ILStr_> ILStr2_;

DatabaseTable::Column::Column(ILStr_ il) {
  auto it = il.begin();
  name = *it++; type = *it++; attr = *it;
}

void DatabaseTable::CreateTable_(MySQLSession& sess,
    const std::string& name) const {
  std::string query("CREATE TABLE " + name + " (");
  for (auto& i : columns_) {
    query += " `";
    query += i.name; query += "` ";
    query += i.type; query.push_back(' ');
    query += i.attr; query.push_back(',');
  }
  int cnt = 0;
  for (auto& i : indices_) {
    query += "KEY idx_" + std::to_string(cnt++) + " (";
    for (auto& j : i) query += j + ", ";
    query[query.size() - 2] = ')';
    query.back() = ',';
  }
  if (additional_.size()) {
    query += additional_;
  } else {
    query.pop_back();
  }
  query += " );";
  sess.sql(query).execute();
}

DatabaseTable::DatabaseTable(std::string name, ILStr2_ col, ILStr2_ ind,
    std::string ad) : name_(name), columns_(col.begin(), col.end()),
        indices_(ind.begin(), ind.end()), additional_(ad) {
  std::invalid_argument err("Invalid table format");
  if (!IsValidName(name_) || !columns_.size()) throw err;
  int cnt = 0;
  for (const Column& i : columns_) {
    if (!IsValidName(i.name)) throw err;
    auto it = column_mp_.insert({i.name, cnt++});
    if (!it.second) throw err;
  }
  for (const Index& i : indices_) {
    if (i.empty()) throw err;
    for (const std::string& index : i) {
      if (!column_mp_.count(index)) throw err;
    }
  }
  // No check for type and constraints
}

const std::string& DatabaseTable::operator[](size_t pos) const {
  if (pos >= columns_.size()) {
    throw std::out_of_range("column index out of range");
  }
  return columns_[pos].name;
}

int DatabaseTable::operator[](const std::string& str) const {
  auto it = column_mp_.find(str);
  if (it == column_mp_.end()) throw std::out_of_range("column not exist");
  return it->second;
}

void DatabaseTable::CreateTable(MySQLSession& sess) const {
  CreateTable_(sess, name_);
}

std::string GetSchema(MySQLSession& sess, const std::string& table) {
  try {
    return sess.sql("SHOW CREATE TABLE " + table + ";").execute().fetchOne()[1];
  } catch (const mysqlx::Error&) { return ""; }
}

bool DatabaseTable::IsExist(MySQLSession& sess) const {
  return GetSchema(sess, name_).size();
}

bool DatabaseTable::CheckTable(MySQLSession& sess) const {
  std::minstd_rand gen;
  std::uniform_int_distribution<char> mrand('a', 'z');
  auto randstr = [&]() {
    std::string str;
    for (int i = 0; i < 10; i++) str.push_back(mrand(gen));
    return str;
  };
  std::string tmpname;
  std::string schema1 = GetSchema(sess, name_);
  if (schema1.empty()) return false;

  while (1) {
    tmpname = randstr();
    if (GetSchema(sess, tmpname).empty()) break;
  }
  CreateTable_(sess, tmpname);
  std::string schema2 = GetSchema(sess, tmpname);

  // names of constraints and keys does not matter
  std::regex pat1("CONSTRAINT `[^`]*`");
  std::regex pat2("KEY `[^`]*`");
  schema1 = std::regex_replace(schema1, pat1, "CONSTRAINT ");
  schema2 = std::regex_replace(schema2, pat1, "CONSTRAINT ");
  schema1 = std::regex_replace(schema1, pat2, "KEY ");
  schema2 = std::regex_replace(schema2, pat2, "KEY ");
  size_t pos1 = schema1.find('\n'), pos2 = schema2.find('\n');
  bool ret = schema1.substr(pos1) == schema2.substr(pos2);

  sess.sql("DROP TABLE " + tmpname + ";").execute();
  return ret;
}
