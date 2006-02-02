#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <boost/shared_ptr.hpp>
#include <ostream>
#include <sstream>
#include <string>
#include <map>

namespace ipna {
  class Logger {
  public:

#define LOG_MSG(prio,msg) { std::stringstream s; if (logger->isPriorityLevelEnabled(prio)) { s << msg; logger->log(prio,s.str());} }
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
    typedef boost::shared_ptr<Logger> LoggerPtr;
    
  private:
    static std::map<std::string, boost::shared_ptr<Logger> > loggerMap;
    static char* priorityNameMap[];
    std::ostream *stream;
    std::string name;
    PriorityLevel level;
    static boost::shared_ptr<Logger> newLogger(const std::string& name, PriorityLevel prio);
  protected:
    Logger(const std::string& name, PriorityLevel l);
  public:
    static boost::shared_ptr<Logger> getLogger(const std::string& name);
    static boost::shared_ptr<Logger> getRootLogger();
    virtual ~Logger();
    void setPriority(PriorityLevel l);
    inline PriorityLevel getPriority() { return level; }
    inline std::string& getName() { return name; }

    void log(PriorityLevel logLevel, const std::string& msg);
    bool isPriorityLevelEnabled(PriorityLevel prio);

    inline bool isDebugEnabled() { return isPriorityLevelEnabled(DEBUG); }
    inline bool isInfoEnabled() { return isPriorityLevelEnabled (INFO); }
    inline bool isWarnEnabled() { return isPriorityLevelEnabled (WARN); }
    inline bool isErrorEnabled() { return isPriorityLevelEnabled(ERROR); }
    inline bool isFatalEnabled() { return isPriorityLevelEnabled(FATAL); }

    inline void debug(const std::string& msg) { log(DEBUG, msg); }
    inline void info(const std::string& msg)  { log(INFO , msg); }
    inline void warn(const std::string& msg)  { log(WARN , msg); }
    inline void error(const std::string& msg) { log(ERROR, msg); }
    inline void fatal(const std::string& msg) { log(FATAL, msg); }
  };
}

#endif // LOGGER_HPP
