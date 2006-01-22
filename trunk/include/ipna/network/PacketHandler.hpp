#ifndef PACKET_HANDLER_H
#define PACKET_HANDLER_H

#include <ipna/network/Packet.hpp>

namespace ipna {
  namespace network {
    class PacketHandler {
    protected:
      PacketHandler() {}
    public:
      virtual ~PacketHandler() {}
      virtual bool handlePacket(Packet::PacketPtr packet) = 0;
    };
  }
}

#endif // PACKET_HANDLER_H
