#include <ipna/capture/CaptureProgram.hpp>

#include <signal.h>
#include <stdlib.h>
#include <iostream>

#include <QCoreApplication>

ipna::capture::CaptureProgram program;

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
