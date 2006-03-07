/*
  CapturePacketHandler.hpp - captures records.

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
    class SequenceNumberChecker;
  }

  namespace parser {
    class ParserFactory;
  }
  
  namespace capture {
    class CapturePacketHandler : public ipna::network::PacketHandler {
    public:
      CapturePacketHandler(RecordWriter::RecordWriterPtr writer, unsigned int queueSize = 1024);
      virtual ~CapturePacketHandler();
      virtual bool handlePacket(ipna::network::Packet::PacketPtr packet);
    private:
      static ipna::Logger::LoggerPtr logger;
      boost::shared_ptr<ipna::network::SequenceNumberChecker> sequenceChecker;
      boost::shared_ptr<ipna::parser::ParserFactory> parserFactory;
      ipna::parser::PacketParser::RecordVectorPtr records;
      boost::shared_ptr<RecordWriter> _recordWriter;
      unsigned int _queueSize;
    };
  }
}

#endif // CAPTURE_PACKET_HANDLER_H
