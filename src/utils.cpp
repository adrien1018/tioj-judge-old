#include "utils.h"

#include <regex>

bool IsValidName(const std::string& str) {
  if (str.size() < 1 || str.size() > 64) return false;
  return !std::regex_search(str, std::regex("(^\\d)|\\W"));
}
