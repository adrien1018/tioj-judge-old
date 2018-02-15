#ifndef JUDGE_METHOD_H_
#define JUDGE_METHOD_H_

// This header defines function to generate commonly-used judge methods.

#include "types.h"

enum CommonLanguages {
  kLangNULL = 0,  // indicates to skip the stage
  kLangC99,
  kLangC11,
  kLangCpp98,
  kLangCpp11,
  kLangCpp14,
  kLangCpp17,
  kLangCSharp,
  kLangFortran,
  kLangHaskell,
  kLangJava,
  kLangJavaScript,
  kLangPascal,
  kLangPerl,
  kLangPython2,
  kLangPython3,
  kLangRuby,
  kLangRust
};

enum CommonExecMethod {
  kExecNormal,              // 1 user file
  kExecInteractive,         // 1 user file, 1 judge header
  kExecIOIStyle,            // 1 user impl, 1 judge file+header (impl/file is unordered)
  kExecMultiPhase,          // arg user impl, arg judge file+header
  kExecMultiPhaseCombined,  // 1 user impl, 1 judge file+header (dif. argv)
  kExecOutputOnly,          // arg user files, 1 judge file (dif. argv)
  kExecCFInteractive,       // 1 user file, 1 judge file
};

enum CommonEvalMethod {
  kEvalStrictCompare,
  kEvalNormalCompare,
  kEvalSpecialJudge,
  kEvalOldSpecialJudge
};

struct CommonOpts {
  CommonLanguages code_check_lang;
  CommonExecMethod exec_method;
  size_t arg;
  CommonLanguages main_lang, main_lang2;  // CFInteractive can have different langs
  CommonEvalMethod eval_method;
  CommonLanguages eval_lang;  // used only if eval_method is special judge
  CommonLanguages score_lang;
  CommonLanguages post_score_lang;
};

Task GenerateCompileMethod(CommonLanguages lang, const std::string& code,
                           const std::string& output_file, bool no_link);
Task GenerateMethod(const CommonOpts& opts, Stage stage);

#endif
