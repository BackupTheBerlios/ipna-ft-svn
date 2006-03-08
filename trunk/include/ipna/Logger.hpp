/*
  Logger.hpp - a class that represents a logger

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
#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <boost/shared_ptr.hpp>
#include <ostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

namespace ipna {
  class Logger {
  public:
    typedef std::map<std::string, boost::shared_ptr<Logger> > LoggerMap;
    typedef boost::shared_ptr<Logger> LoggerPtr;

#define LOG_MSG(prio,msg) { std::stringstream s; if (logger->isPriorityLevelEnabled(prio)) { s /*<< __PRETTY_FUNCTION__ << ":"*/ << msg; logger->log(prio,s.str());} }
#define LOG_DEBUG(msg) LOG_MSG(::ipna::Logger::DEBUG, msg)
#define LOG_FATAL(msg) LOG_MSG(::ipna::Logger::FATAL, msg)
#define LOG_INFO(msg) LOG_MSG(::ipna::Logger::INFO, msg)
#define LOG_ERROR(msg) LOG_MSG(::ipna::Logger::ERROR, msg)
#define LOG_WARN(msg) LOG_MSG(::ipna::Logger::WARN, msg)

    enum PriorityLevel {
      FATAL,
      ERROR,
      WARN,
      INFO,
      DEBUG,
      UNKNOWN
    };
    
  private:
    static char* priorityNameMap[];
    std::ostream *stream;
    std::string name;
    PriorityLevel level;
    static boost::shared_ptr<Logger> newLogger(const std::string& name, PriorityLevel prio);
    std::ofstream dev_null;
  protected:
    explicit Logger(const std::string& name, PriorityLevel l);
    static LoggerMap& getLoggerMap();
    
  public:
    static boost::shared_ptr<Logger> getLogger(const std::string& name);
    static boost::shared_ptr<Logger> getRootLogger();
    virtual ~Logger();
    void setPriority(PriorityLevel l);

    inline PriorityLevel getPriority() { return level; }
    inline std::string& getName() { return name; }

    void log(PriorityLevel logLevel, const std::string& msg);
    std::ostream& log(PriorityLevel logLevel);
    bool isPriorityLevelEnabled(PriorityLevel prio);

    inline bool isDebugEnabled() { return isPriorityLevelEnabled(DEBUG); }
    inline bool isInfoEnabled() {  return isPriorityLevelEnabled(INFO); }
    inline bool isWarnEnabled() {  return isPriorityLevelEnabled(WARN); }
    inline bool isErrorEnabled() { return isPriorityLevelEnabled(ERROR); }
    inline bool isFatalEnabled() { return isPriorityLevelEnabled(FATAL); }

    inline void debug(const std::string& msg) { log(DEBUG, msg); }
    inline void info(const std::string& msg)  { log(INFO , msg); }
    inline void warn(const std::string& msg)  { log(WARN , msg); }
    inline void error(const std::string& msg) { log(ERROR, msg); }
    inline void fatal(const std::string& msg) { log(FATAL, msg); }

    inline std::ostream& debug() { return log(DEBUG); }
    inline std::ostream& info()  { return log(INFO ); }
    inline std::ostream& warn()  { return log(WARN ); }
    inline std::ostream& error() { return log(ERROR); }
    inline std::ostream& fatal() { return log(FATAL ); }
  };
}

#endif // LOGGER_HPP
