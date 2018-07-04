#include "mysql_helper.h"

#include <cstdio>

// ----- MySQLSession -----

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

mysqlx::SqlStatement MySQLSession::sql(const std::string& query) {
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

bool DatabaseTable::IsExist(MySQLSession& sess) const {
  try {
    sess.sql("SHOW COLUMNS FROM " + name_ + ";").execute();
  } catch (const mysqlx::Error&) { return false; }
  return true;
}

const std::string& DatabaseTable::operator[](size_t pos) const {
  return columns_[pos].name;
}

int DatabaseTable::operator[](const std::string& str) const {
  auto it = column_mp_.find(str);
  if (it == column_mp_.end()) return -1;
  return it->second;
}

void DatabaseTable::CreateTable(MySQLSession& sess) const {
  std::string query("CREATE TABLE " + name_ + " (");
  for (auto& i : columns_) {
    query.push_back(' ');
    query += i.name; query.push_back(' ');
    query += i.type; query.push_back(' ');
    query += i.attr; query.push_back(',');
  }
  if (additional_.size()) {
    query += additional_;
  } else {
    query.pop_back();
  }
  query += " );";
  sess.sql(query).execute();

  int cnt = 0;
  for (auto& i : indices_) {
    query = "CREATE INDEX idx_" + std::to_string(cnt++) + " ON " + name_ + "(";
    for (auto& j : i) query += j + ", ";
    query[query.size() - 2] = ')';
    query.back() = ';';
    sess.sql(query).execute();
  }
}

