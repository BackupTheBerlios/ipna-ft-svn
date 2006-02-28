#ifndef FANOUT_PACKET_HANDLER_H
#define FANOUT_PACKET_HANDLER_H

#include <vector>

#include <ipna/network/HostPort.hpp>
#include <ipna/network/PacketHandler.hpp>
#include <ipna/network/Packet.hpp>
#include <ipna/network/HostPort.hpp>
#include <ipna/Logger.hpp>

#include <QUdpSocket>
#include <QHostAddress>

namespace ipna {
  namespace network {
    class SequenceNumberChecker;
  }
  
  namespace fanout {
    class FanoutPacketHandler : public ipna::network::PacketHandler {
    public:
      FanoutPacketHandler(boost::shared_ptr<QUdpSocket> s);
      virtual ~FanoutPacketHandler();
      virtual bool handlePacket(network::Packet::PacketPtr packet);
      virtual FanoutPacketHandler* addDestination(const network::HostPort& hp);
      virtual FanoutPacketHandler* addEngineMapping(const network::HostPort& hp);
    protected:
      virtual bool setEngineId(network::Packet::PacketPtr packet);
    private:
      static ipna::Logger::LoggerPtr logger;
      boost::shared_ptr<QUdpSocket> socket;
      boost::shared_ptr<ipna::network::SequenceNumberChecker> sequenceChecker;
      std::vector<network::HostPort> destinations;
      std::vector<network::HostPort> engineMapping;
    };
  }
}

#endif // FANOUT_PACKET_HANDLER_H
