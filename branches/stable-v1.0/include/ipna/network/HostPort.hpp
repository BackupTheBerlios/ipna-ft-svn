#ifndef HOST_PORT_HPP
#define HOST_PORT_HPP

#include <string>
#include <QHostAddress>

#include <ipna/Logger.hpp>

namespace ipna {
  namespace network {
    class HostPort {
    public:
      HostPort(const std::string& s, bool useIPv6 = false);
      virtual ~HostPort() {}
      QHostAddress host;
      unsigned short port;
    private:
      static ipna::Logger::LoggerPtr logger;
    };
  }
}

#endif // HOST_PORT_HPP
    
