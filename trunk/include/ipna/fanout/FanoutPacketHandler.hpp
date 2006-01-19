#ifndef FANOUT_PACKET_HANDLER_H
#define FANOUT_PACKET_HANDLER_H

#include <boost/shared_ptr.hpp>
#include <vector>

#include <ipna/network/PacketHandler.hpp>
#include <ipna/Logger.hpp>

namespace ipna {
  namespace network {
    class Socket;
    class SequenceNumberChecker;
  }
  
  namespace fanout {
    class FanoutPacketHandler : public ipna::network::PacketHandler {
    public:
      typedef boost::shared_ptr<struct sockaddr_in> DestinationPtr;
      typedef std::vector<DestinationPtr>::iterator DestinationIterator;
      
      FanoutPacketHandler(boost::shared_ptr<ipna::network::Socket> s);
      virtual ~FanoutPacketHandler();
      virtual bool handlePacket(Packet packet, int len, struct sockaddr_in & from);
      virtual FanoutPacketHandler* addDestination(DestinationPtr d);
    private:
      static ipna::Logger::LoggerPtr logger;
      boost::shared_ptr<ipna::network::Socket> socket;
      boost::shared_ptr<ipna::network::SequenceNumberChecker> sequenceChecker;
      std::vector<DestinationPtr> destinations;
    };
  }
}

#endif // FANOUT_PACKET_HANDLER_H
