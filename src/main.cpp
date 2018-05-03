

void ParseArgs(int argc, char** argv) {
  // Command line parsing
}

void SignalHandler(int signo) {

}

void JudgeStart() {
  // Build IPC (between communicator and scheduler)
  // fork & exec communicator and scheduler
}

int main(int argc, char** argv) {
  ParseArgs(argc, argv);

  // Signal handling

  while (true) {
    JudgeStart();

    // wait for signal
  }
}
