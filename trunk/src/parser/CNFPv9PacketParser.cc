#include <ipna/parser/CNFPv9PacketParser.hpp>
#include <vector>

using namespace ipna;
using namespace ipna::network;
using namespace ipna::parser;

CNFPv9PacketParser::CNFPv9PacketParser() : PacketParser() {}

PacketParser::RecordVectorPtr
CNFPv9PacketParser::parse(Packet::PacketPtr packet) {
  PacketParser::RecordVectorPtr records(new PacketParser::RecordVector);
  return records;
}
  
