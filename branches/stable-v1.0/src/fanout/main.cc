#include <ipna/fanout/FanoutProgram.hpp>

#include <stdlib.h>
#include <signal.h>

ipna::fanout::FanoutProgram program;

void stop() {
  program.stop();
}

void handleSignal(int sig) {
  exit(0);
}

int main(int argc, char** argv) {
  program.initialize(argc,argv);

  atexit(stop);
  signal(SIGTERM,handleSignal);
  signal(SIGINT,handleSignal);

  program.start();
}
