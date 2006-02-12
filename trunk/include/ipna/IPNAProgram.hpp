#ifndef IPNA_PROGRAM_HPP
#define IPNA_PROGRAM_HPP

#include <string>
#include <iostream>
#include <boost/program_options.hpp>

#include <ipna/Logger.hpp>

namespace ipna {
  namespace po = boost::program_options;

  class IPNAProgram {
  public:
    IPNAProgram(const std::string& name = "ipna-ft");

    virtual ~IPNAProgram() {}

    virtual void initialize(int argc, char **argv);

    virtual void start() = 0;
    
    inline po::options_description& getOptions() {
      return _options;
    }
    inline po::positional_options_description& getPositionalOptions() {
      return _positionalOptions;
    }
    inline po::variables_map& getArgumentMap() {
      return _arguments;
    }
    inline bool isSet(const std::string& arg) {
      return _arguments.count(arg) > 0;
    }
    inline int verbosity() {
      return isSet("verbose");
    }
    inline void printHelp(std::ostream& os = std::cerr) {
      os << _options << std::endl;
    }
  protected:
    static Logger::LoggerPtr logger;
    
  private:
    po::options_description _options;
    po::positional_options_description _positionalOptions;
    po::variables_map _arguments;
  };
}

#endif
