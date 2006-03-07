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
#ifndef PARSER_FACTORY_HPP
#define PARSER_FACTORY_HPP

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

#include <ipna/parser/cnfp.hpp>
#include <map>

namespace ipna {
  namespace parser {
    class ParserFactory;
    class PacketParser;

    typedef boost::shared_ptr<ParserFactory> ParserFactoryPtr;
    typedef boost::shared_ptr<PacketParser> ParserPtr;
    
    class ParserFactory {
    public:
      static ParserFactoryPtr getInstance();
      virtual ~ParserFactory() {}

      ParserPtr getParser(NetflowVersion version);
      ParserPtr getParser(unsigned short version);
      ParserPtr getParser(const char * packet);
      ParserPtr getParser(boost::shared_array<char> packet);
      ParserPtr getParser(const cnfp_common_hdr& hdr);
    protected:
      ParserFactory() {}
      ParserPtr newParser(NetflowVersion version) const;
      typedef std::map<NetflowVersion, ParserPtr> ParserMap;
      ParserMap _parserMap;
    private:
      static ParserFactoryPtr _factory;
    };
  }
}

#endif // PARSER_FACTORY_HPP
