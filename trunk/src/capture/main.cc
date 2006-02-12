#include <ipna/capture/CaptureProgram.hpp>

int main(int argc, char** argv) {
  ipna::capture::CaptureProgram program;
  program.initialize(argc,argv);
  program.start();
}
