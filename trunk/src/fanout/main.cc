#include <ipna/fanout/FanoutProgram.hpp>

int main(int argc, char** argv) {
  ipna::fanout::FanoutProgram program;
  program.initialize(argc,argv);
  program.start();
}
