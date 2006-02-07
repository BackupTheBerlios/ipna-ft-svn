#ifndef FANOUT_PACKET_HANDLER_H
#define FANOUT_PACKET_HANDLER_H

#include <boost/shared_ptr.hpp>
#include <vector>

#include <ipna/capture/RecordWriter.hpp>
#include <ipna/network/PacketHandler.hpp>
#include <ipna/network/Packet.hpp>
#include <ipna/Logger.hpp>
#include <ipna/parser/PacketParser.hpp>

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
      CapturePacketHandler(RecordWriter::RecordWriterPtr writer);
      virtual ~CapturePacketHandler();
      virtual bool handlePacket(ipna::network::Packet::PacketPtr packet);
    private:
      static ipna::Logger::LoggerPtr logger;
      boost::shared_ptr<ipna::network::SequenceNumberChecker> sequenceChecker;
      boost::shared_ptr<ipna::parser::ParserFactory> parserFactory;
      ipna::parser::PacketParser::RecordVectorPtr records;
      boost::shared_ptr<RecordWriter> _recordWriter;
    };
  }
}

#endif // CAPTURE_PACKET_HANDLER_H
