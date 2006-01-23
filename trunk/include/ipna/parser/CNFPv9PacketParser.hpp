#ifndef CNFPv9_PACKET_PARSER_HPP
#define CNFPv9_PACKET_PARSER_HPP

#include <boost/shared_ptr.hpp>
#include <vector>

#include <ipna/network/PacketHandler.hpp>
#include <ipna/network/Packet.hpp>

#include <ipna/parser/PacketParser.hpp>

namespace ipna {
  namespace parser {
    class CNFPv9PacketParser : public PacketParser {
    public:
      CNFPv9PacketParser();
      virtual ~CNFPv9PacketParser() { }
      
      virtual PacketParser::RecordVectorPtr parse(network::Packet::PacketPtr packet);
    private:
      
    };
  }
}
#endif // CNFPv9_PACKET_PARSER_HPP
