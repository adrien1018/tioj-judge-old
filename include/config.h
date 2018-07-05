#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>
#include <vector>

extern const std::string kMySQLHostname;
extern const int kMySQLPort;
extern const std::string kMySQLUsername, kMySQLPassword;
extern const std::string kDatabaseName;
extern const std::string kBoxPath, kDataPath;

// used in file structures and custom judge options
enum Language {
  kLangNull    = 0,
  kLangC       = 1,
  kLangCpp     = 2,
  kLangPython2 = 3,
  kLangPython3 = 4,
  kLangHaskell = 5
};

// used in judging
enum DefaultLanguage {
  kDefLangC11     = 1,
  kDefLangC99     = 2,
  kDefLangC90     = 3,
  kDefLangCpp17   = 4,
  kDefLangCpp14   = 5,
  kDefLangCpp11   = 6,
  kDefLangCpp98   = 7,
  kDefLangPython2 = 8,
  kDefLangPython3 = 9,
  kDefLangHaskell = 10
};

// First work on these languages, and add more when the details are finished

// Path to binary files
extern const std::string kGccPath;     // gcc
extern const std::string kGppPath;     // g++
extern const std::string kPython2Path; // python2
extern const std::string kPython3Path; // python3
extern const std::string kGhcPath;     // ghc

// Syscall list (compile)
extern const std::vector<std::string> kCCppCompileSyscalls;
extern const std::vector<std::string> kPythonCompileSyscalls; // .py -> .pyc
extern const std::vector<std::string> kHaskellCompileSyscalls;

// Syscall list (execution)
extern const std::vector<std::string> kCCppExecSyscalls;
extern const std::vector<std::string> kPythonExecSyscalls;
extern const std::vector<std::string> kHaskellExecSyscalls;

#endif
