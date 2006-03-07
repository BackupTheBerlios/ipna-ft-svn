/*
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
#include <ipna/IPNAProgram.hpp>

#include <fstream>
#include <vector>
#include <iostream>

using namespace ipna;

Logger::LoggerPtr IPNAProgram::logger = Logger::getLogger("ipna");

IPNAProgram::IPNAProgram(const std::string& name)
  : _options("Allowed options") {
  try {
    _options.add_options()
      ("help,h", po::value< std::vector<bool> >()->zero_tokens(), "show this help message")
      ("pidfile,p", po::value<std::string>()->default_value("/tmp/" + name + ".pid"), "use this file to store the pid")
      ("verbose,v", "verbose output")
      ("ipv4,4", "use ipv4")
      ("ipv6,6", "use ipv6, that is the default but currently not implemented!")
      ;
  } catch (po::error & ex) {
    LOG_ERROR(ex.what());
    exit(1);
  }
}

void
IPNAProgram::initialize(int argc, char **argv) {
  try {
    po::store(po::command_line_parser(argc,argv).options(_options).positional(_positionalOptions).run(), _arguments);
    po::notify(_arguments);
  } catch (po::error & ex) {
    LOG_ERROR(ex.what());
    std::cerr << getOptions() << std::endl;
    exit(1);
  } catch (...) {
    LOG_ERROR("unknown things happened!");
    exit(1);
  }

  if (isSet("help")) {
    std::cerr << getOptions() << std::endl;
    exit(0);
  }
  
  if (isSet("verbose")) {
    Logger::getRootLogger()->setPriority(Logger::DEBUG);
  } else {
    Logger::getRootLogger()->setPriority(Logger::INFO);
  }

  std::string pidfile = getArgumentMap()["pidfile"].as<std::string>();
  LOG_DEBUG("using pidfile: \"" << pidfile << "\"");

  try {
    std::ofstream _pid_fstream(pidfile.c_str());
    if (!_pid_fstream.is_open()) {
      throw std::string("could not open file: ").append(pidfile.c_str());
    }
    _pid_fstream << getpid() << std::endl;
    _pid_fstream.close();
  } catch (std::string & ex) {
    LOG_ERROR("could not write pid: " + ex);
    exit(1);
  }
  
}
