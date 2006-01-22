#include <ipna/parser/CNFPv9PacketParser.hpp>
#include <vector>

using namespace ipna;
using namespace ipna::network;
using namespace ipna::parser;

CNFPv9PacketParser::CNFPv9PacketParser() : PacketParser() {}

PacketParser::RecordVector
CNFPv9PacketParser::parse(Packet::PacketPtr packet) {
  return PacketParser::RecordVector(new std::vector<Record::RecordPtr>);
}
  
