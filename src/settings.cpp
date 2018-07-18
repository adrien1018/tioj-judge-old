#include "settings.h"

// class ScoreInt
const int ScoreInt::kBase = 1000000;

ScoreInt::ScoreInt() : val_(0) {}
ScoreInt::ScoreInt(double a) : val_(llround(a * kBase)) {}
ScoreInt::ScoreInt(long double a) : val_(llroundl(a * kBase)) {}
ScoreInt::ScoreInt(int64_t a) : val_(a) {}

ScoreInt& ScoreInt::operator=(double a) {
  val_ = llround(a * kBase);
  return *this;
}

ScoreInt& ScoreInt::operator=(long double a) {
  val_ = llroundl(a * kBase);
  return *this;
}

ScoreInt& ScoreInt::operator=(int64_t a) {
  val_ = a;
  return *this;
}

ScoreInt& ScoreInt::operator+=(const ScoreInt& a) {
  val_ += a.val_;
  return *this;
}

ScoreInt& ScoreInt::operator*=(const ScoreInt& a) {
  val_ = static_cast<long double>(val_) / kBase * a.val_;
  return *this;
}

ScoreInt::operator long double() const {
  return static_cast<long double>(val_) / kBase;
}

ScoreInt::operator int64_t() const {
  return static_cast<int64_t>(val_);
}

ScoreInt operator+(ScoreInt a, const ScoreInt& b) {
  a += b;
  return a;
}

ScoreInt operator*(ScoreInt a, const ScoreInt& b) {
  a += b;
  return a;
}

// Some default constuctors
ProblemSettings::CompileSettings::CompileSettings() :
    lang(kLangNull), args() {}
ProblemSettings::CustomLanguage::CustomLanguage() :
    compile(), as_interpreter(false), tl_a(1.), tl_b(0.), ml_a(1.), ml_b(0.),
    syscall_adj() {}
ProblemSettings::ResultColumn::ResultColumn() :
    type(ProblemSettings::ResultColumn::kScoreFloat), visible(true) {}
ProblemSettings::TestdataFile::TestdataFile() :
    id(0), path() {}
ProblemSettings::CommonFile::CommonFile() :
    usage(ProblemSettings::CommonFile::kLib), lib_lang(kLangNull), id(0),
    stages{0, 0, 0, 0}, file_id(0), path() {}
ProblemSettings::Testdata::Testdata() :
    time_lim(1000000), memory_lim(262144), file_id(), args() {}
ProblemSettings::ScoreRange::ScoreRange() :
    score(), testdata() {}

// Default problem settings
ProblemSettings::ProblemSettings() :
    problem_id(0),
    is_one_stage(false), // 4-stage mode
    code_check_compile(), // no code checking
    custom_lang(), // not used
    execution_type(ProblemSettings::kExecNormal), // batch judge
    execution_times(1), // not used
    lib_compile(), // not used
    pipe_in(false), pipe_out(false), // read from file
    partial_judge(false), // judge whole problem
    evaluation_type(ProblemSettings::kEvalNormal), // normal judge
    evaluation_format(ProblemSettings::kEvalFormatZero), // not used
    password(0),
    evaluation_compile(), // not used
    evaluation_columns(), // no additional columns
    evaluate_nonnormal(false),
    scoring_type(ProblemSettings::kScoringNormal), // normal scoring
    file_per_testdata(2), // no additional files
    testdata_files(), common_files(),
    kill_old(false), // not used
    // custom_stage(),
    timestamp(0) {}
