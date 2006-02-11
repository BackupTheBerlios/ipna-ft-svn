#include <ipna/network/HostPort.hpp>

#include <vector>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace ipna;
using namespace ipna::network;

Logger::LoggerPtr HostPort::logger = Logger::getLogger("ipna.network.HostPort");

template<class T>
T parseString(const std::string& s) {
  std::stringstream strm(s);
  T val;
  strm >> val;
  return val;
}

HostPort::HostPort(const std::string& s, bool useIPv6) {
  std::vector<std::string> splitted;
  boost::split(splitted, s, boost::algorithm::is_any_of("/"));

  if (splitted.size() < 2) {
    LOG_ERROR("invalid ip/port! " << s);
    exit(1);
  }

  if (splitted[0] == "*") {
    if (useIPv6)
      host = QHostAddress::AnyIPv6;
    else
      host = QHostAddress::Any;
  } else {
    host = QHostAddress(QString(splitted[0].c_str()));
  }
  port = parseString<unsigned short>(splitted[1]);
}
