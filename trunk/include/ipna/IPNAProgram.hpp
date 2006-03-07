/*
  IPNAProgram.hpp - a class that represents a complete program,
                    holding some common data.

  Copyright (C) 2006 Alexander Petry

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
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
    virtual void stop() = 0;
    
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
