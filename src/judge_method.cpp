#include "judge_method.h"

const ResLimit kDefaultCompileLim = {
  time_limit : 30ll * 1000000,  // 30 seconds
  rss_limit : 1ll << 30,        // 1 GiB
  vss_limit : ResLimit::kNoLim,
  stack_limit : ResLimit::kNoLim,
  filesize_limit : 1ll << 30,   // 1 GiB
  process_limit : 5  // compiler usually needs to fork
};

void AppendTask(Task& head, Task&& tail)
{
  head.splice(head.end(), std::move(tail));
}

ParTask MakeDefaultParTask(const Program& prog)
{
  return ParTask{
    programs : {prog},
    files : {},
    pipes : {},
    processor_count : 1
  };
}

Environment::PathPair PathPairByName(const std::string& outdir,
                                     const std::string& name) {
  return {outdir + name, name};
}

// helper function to generate C/C++ compilation settings
Task CCppCompileTask(const std::string& compiler,
                     const std::string& standard,
                     const std::vector<Environment::PathPair>& code,
                     const std::vector<Environment::PathPair>& header,
                     const std::string& output) {
  Program prog;
  prog.limits = kDefaultCompileLim;
  // TODO: set prog.env.mounts, prog.env.requirements
  size_t compiler_req_size = prog.env.requirements.size();

  auto AddRequirements = [&prog](const Environment::PathPair& file) {
    prog.env.requirements.push_back({file, true, 0444});
  };

  for (auto& file : header) AddRequirements(file);
  prog.env.results.resize(1);

  if (code.size() == 1) {
    AddRequirements(code[0]);
    prog.env.results[0] = PathPairByName("${TMPDIR}/", output);
    prog.command = {compiler, standard, "-O2", "-static", code[0].path_box,
                    "-o", output};
    return {MakeDefaultParTask(prog)};
  }

  ParTask compile_task;
  compile_task.processor_count = 1;

  size_t count = 0;  // compile each file
  AddRequirements({"", ""});
  prog.command = {compiler, standard, "-O2", "-static", "-c", "", "-o", ""};
  for (auto& file : code) {
    prog.command[5] = file.path_box;
    prog.command[7] = "pre_exec_lib" + std::to_string(count++) + ".o";
    prog.env.requirements.back().file = file;
    prog.env.results[0] = PathPairByName("${STAGEDIR}/", prog.command[7]);
    compile_task.programs.push_back(prog);
  }

  prog.env.requirements.resize(compiler_req_size);  // linking
  prog.env.results[0] = {"${TMPDIR}/" + output, output};
  prog.command = {compiler, "-o", output};
  for (size_t i = 0; i < code.size(); i++) {
    prog.command.push_back("pre_exec_lib" + std::to_string(i) + ".o");
    AddRequirements(PathPairByName("${STAGEDIR}/", prog.command.back()));
  }
  return {compile_task, MakeDefaultParTask(prog)};
}

Task GenerateCompileMethod(CommonLanguages lang,
                           const std::vector<Environment::PathPair>& code,
                           const std::vector<Environment::PathPair>& header,
                           const std::string& output_file) {
  if (lang == kLangJavaScript || lang == kLangPerl ||
      lang == kLangPython2 || lang == kLangPython3 ||
      lang == kLangRuby) {
    // interpreted languages, no need of compilation
    return Task();
  }
  switch (lang) {
    case kLangC99:
      return CCppCompileTask("${gcc}", "-std=c99", code, header, output_file);
    case kLangC11:
      return CCppCompileTask("${gcc}", "-std=c11", code, header, output_file);
    case kLangCpp98:
      return CCppCompileTask("${g++}", "-std=c++98", code, header, output_file);
    case kLangCpp11:
      return CCppCompileTask("${g++}", "-std=c++11", code, header, output_file);
    case kLangCpp14:
      return CCppCompileTask("${g++}", "-std=c++14", code, header, output_file);
    case kLangCpp17:
      return CCppCompileTask("${g++}", "-std=c++17", code, header, output_file);
    // TODO: other languages
    // TODO: write config to generate requirements (by ldd / strace)
  }
}

Environment GenerateExecEnvironment(CommonLanguages lang,
                                    const std::string& file) {
  Environment ret;
  ret.box_permission = 0111; // cannot read directory contents
  switch (lang) {
    case kLangC99: case kLangC11: case kLangCpp98:
    case kLangCpp11: case kLangCpp14: case kLangCpp17: { // C/C++ family
      // can be compiled statically
      break;
    }
    // TODO: other languages
    // TODO: write config to generate requirements (by ldd / strace)
  }
}

Task GenerateMethod(const CommonOpts& opts, Stage stage) {
  auto UserPair = [](const std::string& str)->Environment::PathPair {
    return {"${USRDIR}/" + str, str};
  };
  auto AssetPair = [](const std::string& str)->Environment::PathPair {
    return {"${ASDIR}/" + str, str};
  };

  Task ret;
  switch (stage) {
    case kStagePreExec: {
      auto AddTask = [&ret, &opts](
          CommonLanguages lang,
          const std::vector<Environment::PathPair>& code,
          const std::vector<Environment::PathPair>& header,
          const std::string& output) {
        AppendTask(ret, GenerateCompileMethod(lang, code, header, output));
      };

      if (opts.code_check_lang != kLangNULL) {  // code checker
        AddTask(opts.code_check_lang, {AssetPair("${PREEXEC_0}")}, {},
                "pre_exec");
      }

      switch (opts.exec_method) {  // execution stage
        case kExecNormal: {
          AddTask(opts.main_lang, {UserPair("${USER_0}")}, {}, "exec0");
          break;
        }
        case kExecInteractive: {
          AddTask(opts.main_lang, {UserPair("${USER_0}")},
                  {AssetPair("${EXEC_0}")}, "exec0");
          break;
        }
        case kExecIOIStyle: case kExecMultiPhaseCombined: {
          AddTask(opts.main_lang, {UserPair("${USER_0}"),
                                   AssetPair("${EXEC_1}")},
                  {AssetPair("${EXEC_0}")}, "exec0");
          break;
        }
        case kExecMultiPhase: {
          using std::to_string;
          for (size_t i = 0; i < opts.arg; i++) {
            AddTask(opts.main_lang,
                    {UserPair("${USER_" + to_string(i) + "}"),
                    AssetPair("${EXEC_" + to_string(i * 2 + 1) + "}")},
                    {AssetPair("${EXEC_" + to_string(i * 2) + "}")},
                    "exec" + to_string(i));
          }
        }
        case kExecOutputOnly: {
          AddTask(opts.main_lang, {AssetPair("${EXEC_0}")}, {}, "exec0");
          break;
        }
        case kExecCFInteractive: {
          AddTask(opts.main_lang, {UserPair("${USER_0}")}, {}, "exec0");
          AddTask(opts.main_lang2, {AssetPair("${EXEC_0}")}, {}, "exec1");
          break;
        }
      }
      break;
    }
    case kStageExecution: {
      break;
    }
    case kStageEvaluation: {
      break;
    }
    case kStageScoring: {
      break;
    }
    case kStagePostScoring: {
      break;
    }
    case kStageCompetition: {
      break;
    }
  }
}
