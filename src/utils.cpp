#include "utils.h"

#include <ctime>
#include <cstdlib>
#include <regex>

#define IFERR(x) if ((x) < 0)

void ThrowErrno() {
  throw std::system_error(errno, std::generic_category());
}

// ext2/3/4
bool IsValidFilename(const std::string& str) {
  if (str.size() < 1 || str.size() > 255) return false;
  return !std::regex_search(str, std::regex("[\\0/]"));
}

bool IsDownwardPath(const std::filesystem::path& path) {
  for (auto& i : path) {
    if (i == "..") return false;
  }
  return true;
}

std::string ConcatPath(const std::string& path1, const std::string& path2) {
  if (path2.empty()) return path1;
  if (path1.empty() || path2[0] == '/') return path2;
  if (path1.back() == '/') return path1 + path2;
  return path1 + '/' + path2;
}

// length <= 64 and only alphanumeric and '_'; not start with number
bool IsValidDBName(const std::string& str) {
  if (str.size() < 1 || str.size() > 64) return false;
  return !std::regex_search(str, std::regex("(^\\d)|\\W"));
}

const long long kZeroToEpoch = 62167219200; // in sec

std::string DateTimeStr(long long timestamp) {
  char buf[30];
  time_t tm = timestamp / 1000 - kZeroToEpoch;
  int md = timestamp % 1000;
  strftime(buf, 30, "%F %T", gmtime(&tm));
  std::string res(buf);
  res.push_back('.');
  if (md < 10) res.push_back('0');
  if (md < 100) res.push_back('0');
  return res + std::to_string(md);
}

long long DateTimeVal(const std::string& str) {
  std::regex pat("(\\d{4})-(\\d{2})-(\\d{2}) (\\d{2}):(\\d{2}):(\\d{2})\\.(\\d{3})");
  std::smatch match;
  if (!regex_match(str, match, pat)) {
    throw std::invalid_argument("invalid date format");
  }
  struct tm time_obj;
  time_obj.tm_year = std::stoi(match[1]) - 1900;
  time_obj.tm_mon = std::stoi(match[2]) - 1;
  time_obj.tm_mday = std::stoi(match[3]);
  time_obj.tm_hour = std::stoi(match[4]);
  time_obj.tm_min = std::stoi(match[5]);
  time_obj.tm_sec = std::stoi(match[6]);
  return (static_cast<long long>(timegm(&time_obj)) + kZeroToEpoch) * 1000
      + std::stoi(match[7]);
}

std::vector<std::string> SplitString(const std::string& a) {
  if (!a.size()) return std::vector<std::string>();
  bool flag1 = false, flag2 = false;
  std::vector<std::string> res(1);
  for (char i : a) {
    if (flag1) {
      if (i == '\"') flag1 = false, flag2 = true;
      else res.back().push_back(i);
    }
    else {
      if (i == '\"') {
        flag1 = true;
        if (flag2) flag2 = false, res.back().push_back('\"');
      }
      else if (i == ',') {
        res.push_back("");
        flag2 = false;
      }
      else res.back().push_back(i);
    }
  }
  return res;
}
