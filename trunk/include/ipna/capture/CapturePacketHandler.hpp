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

  namespace parser {
    class ParserFactory;
  }
  
  namespace capture {
    class CapturePacketHandler : public ipna::network::PacketHandler {
    public:
      CapturePacketHandler();
      virtual ~CapturePacketHandler();
      virtual bool handlePacket(Packet packet, int len, struct sockaddr_in & from);
    private:
      static ipna::Logger::LoggerPtr logger;
      boost::shared_ptr<ipna::network::SequenceNumberChecker> sequenceChecker;
      boost::shared_ptr<ipna::parser::ParserFactory> parserFactory;
    };
  }
}

#endif // CAPTURE_PACKET_HANDLER_H
