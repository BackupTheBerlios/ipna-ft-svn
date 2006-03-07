/*
  Copyright (C) 2006 Alexander Petry

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
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
