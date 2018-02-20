#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>
#include <vector>

extern const std::string kBoxPath, kDataPath;
extern const std::string k_gccPath, k_gppPath, k_monoPath, k_mcsPath, k_gfortranPath, k_ghcPath, k_javacPath, k_javaPath, k_nodePath, k_fpcPath, k_perlPath, k_python2Path, k_python3Path, k_rubyPath, k_rustcPath;
extern const std::vector<std::string> kCCppRunSyscalls, kCSharpRunSyscalls, kFortranRunSyscalls, kHaskellRunSyscalls, kJavaRunSyscalls, kJavaScriptRunSyscalls, kPascalRunSyscalls, kPerlRunSyscalls, kPythonRunSyscalls, kRubyRunSyscalls, kRustRunSyscalls;
extern const std::vector<std::string> kCCppCompileSyscalls, kCSharpCompileSyscalls, kFortranCompileSyscalls, kHaskellCompileSyscalls, kJavaCompileSyscalls, kPascalCompileSyscalls, kRustCompileSyscalls;

#endif
