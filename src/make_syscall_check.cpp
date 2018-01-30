#include <iostream>
#include <string>

using namespace std;

int main()
{
  cout <<
    "#include <iostream>\n#include <sstream>\n#include <seccomp.h>\n#include <unistd.h>\n\n"
    "void make_syscall_list() {\n  std::stringstream ans;\n  int nxt = 0;\n"
    "#define chk ans.seekg(0, std::ios::end); "
    "if (ans.tellg() >= nxt) ans << \"\\n    \", nxt += 80; ans.seekg(0, std::ios::beg)\n"
    "#define out(str) ans << \"{\\\"\"#str\"\\\",\" << __NR_##str << \"},\"; chk;\n";
  string str;
  while (cin >> str) cout << "#ifdef __NR_" << str << "\n  out(" << str << ")\n#endif\n";
  cout <<
    "  std::string str = ans.str();\n  str.pop_back();\n"
    "  std::cout << \"#include \\\"config.h\\\"\\n\\n"
    "const std::unordered_map<std::string, int> syscall_num = {\" << str << \"};\\n\";\n"
    "}\n\nint main()\n{\n  make_syscall_list();\n}\n";
}
