#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>
#include <vector>

extern const std::string kBoxPath, kDataPath;

enum Languages {
  kLangC11 = 1,
  kLangC99 = 2,
  kLangC90 = 3,
  kLangCpp17 = 4,
  kLangCpp14 = 5,
  kLangCpp11 = 6,
  kLangCpp98 = 7,
  kLangPython2 = 8,
  kLangPython3 = 9,
  kLangHaskell = 10
};

// First work on these, and add more when the details are finished

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
