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
#include <stdio.h>
#include <sys/socket.h>

#include <algorithm>

#include <ipna/capture/CapturePacketHandler.hpp>
#include <ipna/parser/ParserFactory.hpp>
#include <ipna/parser/PacketParser.hpp>
#include <ipna/network/SequenceNumberChecker.hpp>
#include <ipna/parser/cnfp.hpp>

using namespace std;
using namespace ipna;
using namespace ipna::capture;
using namespace ipna::network;
using namespace ipna::parser;

Logger::LoggerPtr CapturePacketHandler::logger = Logger::getLogger("ipna.capture");

CapturePacketHandler::CapturePacketHandler(RecordWriter::RecordWriterPtr writer, unsigned int queueSize) :
  sequenceChecker(new SequenceNumberChecker()),
  parserFactory(ParserFactory::getInstance()),
  _recordWriter(writer),
  _queueSize(queueSize)
{
  records = PacketParser::RecordVectorPtr(new PacketParser::RecordVector());
}

CapturePacketHandler::~CapturePacketHandler() {
  if (records->size() > 0) {
    _recordWriter->write(records);
    records->clear();
  }
}

bool
CapturePacketHandler::handlePacket(ipna::network::Packet::PacketPtr packet) {
  ParserPtr parser = parserFactory->getParser(packet->getBytes());

  if (!parser->analyze(packet))
    return false;

  size_t seq = parser->getSequenceNumber();
  size_t numNewRecords = parser->parse(packet, records);
  logger->debug() << "got " << numNewRecords << " new records" << std::endl;
  if (records->size() > _queueSize) {
    sort(records->begin(), records->end(), Record::compare_lt);

    _recordWriter->write(records);
    records->clear();
  }
  
  SequenceNumberChecker::SequenceError seq_err =
    sequenceChecker->check(seq);
  if (seq_err == SequenceNumberChecker::SEQ_MISSED) {
    logger->warn() << "missed " << (int)(sequenceChecker->missed()) << " packet(s)" << std::endl;
  }

  return true;
}

bool
CapturePacketHandler::handleTimeout() {
  // notify the writer
  _recordWriter->notifyTimeout();
  return true;
}
