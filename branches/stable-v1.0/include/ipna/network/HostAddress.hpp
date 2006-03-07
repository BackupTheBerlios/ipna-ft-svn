#ifndef HOST_ADDRESS_HPP
#define HOST_ADDRESS_HPP

#include <string>
#include <ostream>
#include <sys/socket.h>
#include <netinet/in.h>

namespace ipna {
  namespace network {
    class HostAddress {
    public:
      enum SpecialAddress {
	Null, LocalHost, LocalHostIPv6, Broadcast, Any, AnyIPv6
      };
      
      HostAddress();
      HostAddress(u_int32_t ipv4Addr);
      HostAddress(const sockaddr_in& sockaddr);
      HostAddress(const std::string& addr);
      HostAddress(const HostAddress& haddr);
      HostAddress(const in6_addr& addr);
      HostAddress(SpecialAddress addr);

      virtual ~HostAddress() {}
      
      void clear();
      std::string toString() const;
      u_int32_t toIPv4Addr() const;

      HostAddress& operator=(const HostAddress& addr);
      HostAddress& operator=(const std::string& addr);
      bool operator==(const HostAddress& other) const;
      bool operator==(const std::string& other) const;
      bool operator==(SpecialAddress other) const;

    private:
      enum AddressType { UNKNOWN, IPv4, IPv6 };
      AddressType _type;
      u_int32_t _ipv4Addr;
      in6_addr _ipv6Addr;
    };

    std::ostream& operator<<(std::ostream& out, const HostAddress& a);
  }
}

#endif // HOST_ADDRESS_HPP
