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
#ifndef CNFPv5_PACKET_PARSER_HPP
#define CNFPv5_PACKET_PARSER_HPP

#include <boost/shared_ptr.hpp>
#include <vector>

#include <ipna/parser/cnfp.hpp>
#include <ipna/network/PacketHandler.hpp>
#include <ipna/network/Packet.hpp>

#include <ipna/parser/PacketParser.hpp>
#include <ipna/Logger.hpp>

namespace ipna {
  namespace parser {
    class CNFPv5PacketParser : public PacketParser {
    public:
      CNFPv5PacketParser();
      virtual ~CNFPv5PacketParser() { }
      
      virtual size_t parse(network::Packet::PacketPtr packet, PacketParser::RecordVectorPtr records);
      virtual bool analyze(network::Packet::PacketPtr packet);
      virtual size_t getSequenceNumber();
    private:
      static ipna::Logger::LoggerPtr logger;
      cnfp_v5_hdr *header;
    };
  }
}
#endif // CNFPv9_PACKET_PARSER_HPP
