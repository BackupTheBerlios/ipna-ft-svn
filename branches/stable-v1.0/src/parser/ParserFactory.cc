#include <iostream>
#include <ipna/parser/ParserFactory.hpp>
#include <ipna/parser/PacketParser.hpp>
#include <ipna/parser/CNFPv9PacketParser.hpp>
#include <utility>

using namespace ipna;
using namespace ipna::parser;

ParserFactoryPtr ParserFactory::_factory(new ParserFactory);

ParserFactoryPtr
ParserFactory::getInstance() { return _factory; }

ParserPtr
ParserFactory::getParser(NetflowVersion version) {
  ParserMap::iterator it = _parserMap.find(version);
  if (it != _parserMap.end()) {
    return it->second;
  } else {
    ParserPtr p = newParser(version);
    _parserMap[version] = p;
    return p;
  }
}

ParserPtr
ParserFactory::getParser(unsigned short version) {
  return getParser((NetflowVersion)version);
}

ParserPtr
ParserFactory::getParser(const char * packet) {
  cnfp_common_hdr hdr = *((cnfp_common_hdr*)packet);
  return getParser(hdr);
}

ParserPtr
ParserFactory::getParser(const cnfp_common_hdr& hdr) {
  return getParser(ntohs(hdr.version));
}

ParserPtr
ParserFactory::getParser(boost::shared_array<char> packet) {
  return getParser(packet.get());
}

ParserPtr
ParserFactory::newParser(NetflowVersion version) const {
  ParserPtr p;
  
  switch (version) {
  case CNFPv1:
  case CNFPv5:
  case CNFPv7:
  case CNFPv8:
  case CNFPv9:
    //    std::cerr << "returning new v9 parser" << std::endl;
    p = boost::shared_ptr<PacketParser>(new CNFPv9PacketParser());
  default:
    break;
  }
  
  return p;
}
