#ifndef PACKET_HANDLER_H
#define PACKET_HANDLER_H

#include <boost/shared_array.hpp>

struct sockaddr_in;
namespace ipna {
  class PacketHandler {
  protected:
    PacketHandler() {}
  public:
    typedef boost::shared_array<char> Packet;
    
    virtual ~PacketHandler() {}
    virtual bool handlePacket(Packet packet, int len, struct sockaddr_in & from) = 0;
  };
}

#endif // PACKET_HANDLER_H
