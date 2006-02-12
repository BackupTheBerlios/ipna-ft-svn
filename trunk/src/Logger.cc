#include <iostream>
#include <utility>
#include <cassert>
#include <time.h>

#include <ipna/Logger.hpp>

using namespace ipna;
using namespace std;

typedef map<string, boost::shared_ptr<Logger> > LoggerMap;

LoggerMap Logger::loggerMap;
//Logger::LoggerPtr Logger::_root = Logger::newLogger("root", Logger::FATAL);

char* Logger::priorityNameMap[] = { "FATAL", "ERROR", "WARN", "INFO", "DEBUG", "UNKNOWN" };

Logger::Logger(const string& name, Logger::PriorityLevel l)
  : name(name), level(l){
  stream = &clog;
}

Logger::~Logger() {}

Logger::LoggerPtr
Logger::getLogger(const std::string& name) {
  LoggerMap::iterator it = loggerMap.find(name);
  if (it != loggerMap.end()) {
    return it->second;
  } else {
    return newLogger(name,getRootLogger()->getPriority());
  }
}

Logger::LoggerPtr
Logger::newLogger(const std::string& name, PriorityLevel prio) {
  LoggerPtr logger(new Logger(name,prio));
  loggerMap.insert(make_pair<string,boost::shared_ptr<Logger> >(name,logger));
  return logger;
}

Logger::LoggerPtr
Logger::getRootLogger() {
  string name = "root";
  LoggerMap::iterator it = loggerMap.find(name);
  if (it != loggerMap.end()) {
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

void
Logger::setPriority(Logger::PriorityLevel prio) {
  LoggerPtr root = getRootLogger();
  level = prio;
  if (this == root.get()) {
    for (LoggerMap::iterator it = loggerMap.begin(); it != loggerMap.end(); it++) {
      if (it->second.get() != this) {
	it->second->setPriority(prio);
      }
    }
  }
}
