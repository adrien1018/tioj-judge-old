#ifndef JUDGE_METHOD_H_
#define JUDGE_METHOD_H_

// This header defines function to generate commonly-used judge methods.

#include "types.h"

enum CommonLanguages {
  kNULL = 0,  // indicates to skip the stage
  kC99,
  kC11,
  kCpp98,
  kCpp11,
  kCpp14,
  kCpp17,
  kCSharp,
  kHaskell,
  kJava,
  kJavaScript,
  kPascal,
  kPython2,
  kPython3,
  kRuby,
  kRust
};

enum CommonExecMethod {
  kNormal,              // 1 user file
  kInteractive,         // 1 user file, 1 judge header
  kIOIStyle,            // 1 user header, 1 judge file
  kMultiPhase,          // arg user headers, arg judge files
  kMultiPhaseCombined,  // 1 user header, 1 judge file (dif. argv)
  kOutputOnly,          // arg user files, 1 judge file (dif. argv)
  kCFInteractive        // 1 user file, 1 judge file
};

enum CommonEvalMethod {
  kStrictCompare,
  kNormalCompare,
  kSpecialJudge,
  kOldSpecialJudge
};

struct CommonOpts {
  CommonLanguages code_check_lang;
  CommonExecMethod exec_method;
  size_t arg;
  CommonLanguages main_lang;
  CommonEvalMethod eval_method;
  CommonLanguages eval_lang;  // used only if eval_method is special judge
  CommonLanguages score_lang;
  CommonLanguages post_score_lang;
};

Task GenerateMethod(const CommonOpts& opts, Stage stage);

#endif
