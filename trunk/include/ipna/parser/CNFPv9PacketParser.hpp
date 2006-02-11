#ifndef CNFPv9_PACKET_PARSER_HPP
#define CNFPv9_PACKET_PARSER_HPP

#include <boost/shared_ptr.hpp>
#include <vector>

#include <ipna/parser/cnfp.hpp>
#include <ipna/network/PacketHandler.hpp>
#include <ipna/network/Packet.hpp>

#include <ipna/parser/PacketParser.hpp>
#include <ipna/Logger.hpp>

namespace ipna {
  namespace parser {
    class CNFPv9PacketParser : public PacketParser {
    public:
      CNFPv9PacketParser();
      virtual ~CNFPv9PacketParser() { }
      
      virtual size_t parse(network::Packet::PacketPtr packet, PacketParser::RecordVectorPtr records);
      virtual bool analyze(network::Packet::PacketPtr packet);
      virtual size_t getSequenceNumber();
    private:
      static ipna::Logger::LoggerPtr logger;
      cnfp_v9_hdr_s _header;
    };
  }
}
#endif // CNFPv9_PACKET_PARSER_HPP
