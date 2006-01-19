#include <ipna/network/HostAddress.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;
using namespace ipna;
using namespace ipna::network;

HostAddress::HostAddress()
  : _type(IPv4), _ipv4Addr(0) {}

HostAddress::HostAddress(u_int32_t ipv4Addr)
  : _type(IPv4), _ipv4Addr(ipv4Addr) {}

HostAddress::HostAddress(const sockaddr_in* _sockaddr) {
  if (_sockaddr && _sockaddr->sin_family == AF_INET) {
    _type = IPv4;
    _ipv4Addr = ntohl(_sockaddr->sin_addr.s_addr);
  } else {
    _type = IPv4;
    _ipv4Addr = 0;
  }
}

HostAddress::HostAddress(const std::string& addr) {
  // check if addr contains a '.'
  if (addr.find(".") < addr.size()) {
    in_addr a;
    if (inet_aton(addr.c_str(), &a)) {
      _type = IPv4;
      _ipv4Addr = a.s_addr;
    } else {
      _type = UNKNOWN;
    }
  } else if (addr.find(":") < addr.size()) {
    _type = IPv6;
    // ...
  } else {
    _type = UNKNOWN;
  }
}

HostAddress::HostAddress(const HostAddress& addr) {
  _type = addr._type;
  _ipv4Addr = addr._ipv4Addr;
  _ipv6Addr = addr._ipv6Addr;
}

HostAddress::HostAddress(HostAddress::SpecialAddress addr) {
  switch (addr) {
  case NULL:
    _type = IPv4;
    _ipv4Addr = 0;
    break;
  case LocalHost:
    _type = IPv4;
    break;
  case LocalHostIPv6:
    break;
  case Broadcast:
    break;
  case Any:
    break;
  case AnyIPv6:
    break;
  }
}

void
HostAddress::clear() {
  _type = IPv4;
  _ipv4Addr = 0;
}

string
HostAddress::toString() const {
  switch (_type) {
  case IPv4:
    in_addr a;
    a.s_addr = _ipv4Addr;
    return string(inet_ntoa(a));
  case IPv6:
    return "";
  case UNKNOWN:
  default:
    return "";
  }
}

HostAddress&
HostAddress::operator=(const HostAddress& addr) {
  this->_type = addr._type;
  this->_ipv4Addr = addr._ipv4Addr;
  this->_ipv6Addr = addr._ipv6Addr;
  return *this;
}

HostAddress&
HostAddress::operator=(const std::string& addr) {
  HostAddress tmp(addr);
  return ((*this) = tmp);
}

bool
HostAddress::operator==(const HostAddress& other) const {
  return (this->_type == other._type &&
	  (this->_ipv4Addr == other._ipv4Addr));
  /* ||
  this->_ipv6Addr == other._ipv6Addr));*/
}

bool
HostAddress::operator==(const string& other) const {
  HostAddress tmp(other);
  return (*this) == tmp;
}

bool
HostAddress::operator==(HostAddress::SpecialAddress other) const {
  HostAddress tmp(other);
  return (*this) == tmp;
}
