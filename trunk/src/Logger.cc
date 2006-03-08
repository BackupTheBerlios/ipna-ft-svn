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
#include <iostream>
#include <utility>
#include <cassert>
#include <time.h>

#include <ipna/Logger.hpp>

using namespace ipna;
using namespace std;

char* Logger::priorityNameMap[] = { "FATAL", "ERROR", "WARN", "INFO", "DEBUG", "UNKNOWN" };

Logger::Logger(const string& name, Logger::PriorityLevel l)
  : name(name), level(l), dev_null("/dev/null") {
  stream = &clog;
}

Logger::~Logger() {}

Logger::LoggerMap&
Logger::getLoggerMap() {
  static LoggerMap loggerMap;
  return loggerMap;
}

Logger::LoggerPtr
Logger::getLogger(const std::string& name) {
  LoggerMap::iterator it = getLoggerMap().find(name);
  if (it != getLoggerMap().end()) {
    return it->second;
  } else {
    return newLogger(name,getRootLogger()->getPriority());
  }
}

Logger::LoggerPtr
Logger::newLogger(const std::string& name, PriorityLevel prio) {
  LoggerPtr logger(new Logger(name,prio));
  getLoggerMap().insert(make_pair<string,boost::shared_ptr<Logger> >(name,logger));
  return logger;
}

Logger::LoggerPtr
Logger::getRootLogger() {
  string name = "root";
  LoggerMap::iterator it = getLoggerMap().find(name);
  if (it != getLoggerMap().end()) {
    return it->second;
  } else {
    return newLogger(name, WARN);
  }
}

bool
Logger::isPriorityLevelEnabled(PriorityLevel prio) {
  return (getPriority() >= prio);
}

void
Logger::log(PriorityLevel logLevel, const string& msg ) {
  if (isPriorityLevelEnabled(logLevel)) {
    *stream << "[" << time(NULL) << "] " << priorityNameMap[logLevel] << " - " << getName() << ": " << msg << endl;
  }
}

ostream&
Logger::log(PriorityLevel logLevel) {
  if (isPriorityLevelEnabled(logLevel) && stream != NULL) {
    return *stream << "[" << time(NULL) << "]" << priorityNameMap[logLevel] << " - " << getName() << ": ";
  } else {
    return dev_null;
  }
}

void
Logger::setPriority(Logger::PriorityLevel prio) {
  LoggerPtr root = getRootLogger();
  level = prio;
  if (this == root.get()) {
    for (LoggerMap::iterator it = getLoggerMap().begin(); it != getLoggerMap().end(); it++) {
      if (it->second.get() != this) {
	it->second->setPriority(prio);
      }
    }
  }
}
