#ifndef MYSQL_HELPER_H_
#define MYSQL_HELPER_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <initializer_list>

#include <mysqlx/xdevapi.h>

#include "utils.h"

// MySQL Session wrapper to support late construction
class MySQLSession {
  mysqlx::Session* sess_;
  void CheckNull_() const;

  // C++-style sprintf
  template <class... T> std::string Format_(const std::string&, T...);
public:
  MySQLSession();
  MySQLSession(const MySQLSession&) = delete;
  MySQLSession(MySQLSession&&);
  void operator=(const MySQLSession&) = delete;
  void operator=(MySQLSession&&);
  ~MySQLSession();

  // start / restart a session
  template <class... T> void Start(T...);
  void Stop();

  // change to a database, if not exist then create it (if create)
  void ChangeDatabase(const std::string&, bool create = true);

  // allow sprintf-like usage
  template <class... T> mysqlx::SqlStatement sql(const std::string&, T...);
  mysqlx::SqlStatement sql(const std::string&);
};

template <class T> T _Convert(const T& obj) { return obj; }
// inline is necessary
inline const char* _Convert(const std::string& obj) { return obj.c_str(); }

template <class... T>
std::string MySQLSession::Format_(const std::string& str, T... args) {
  char* strp;
  if (asprintf(&strp, str.c_str(), _Convert(args)...) == -1) {
    throw std::runtime_error("asprintf failed");
  }
  std::string res(strp);
  free(strp);
  return res;
}

template <class... T>
void MySQLSession::Start(T... args) {
  if (sess_) delete sess_;
  sess_ = new mysqlx::Session(args...);
}

template <class... T>
mysqlx::SqlStatement MySQLSession::sql(const std::string& query, T... args) {
  CheckNull_();
  _DEBUG2("query", Format_(query, args...));
  return sess_->sql(Format_(query, args...));
}

// Schema of a MySQL table
class DatabaseTable {
  typedef std::initializer_list<std::string> ILStr_;
  typedef std::initializer_list<ILStr_> ILStr2_;
public:
  struct Column {
    std::string name, type, attr;
    Column(ILStr_);
  };
  typedef std::vector<std::string> Index;
private:
  std::string name_;
  std::vector<Column> columns_;
  std::vector<Index> indices_;
  std::string additional_;
  std::unordered_map<std::string, int> column_mp_;

  void CreateTable_(MySQLSession&, const std::string&) const;
public:
  DatabaseTable(std::string, ILStr2_, ILStr2_, std::string);
  const std::string& operator[](size_t) const;
  int operator[](const std::string&) const;

  // Create the table
  void CreateTable(MySQLSession&) const;

  // Check if the table exists
  bool IsExist(MySQLSession&) const;

  // Check if the table exists and is the same as the schema
  bool CheckTable(MySQLSession&) const;
};

#endif
