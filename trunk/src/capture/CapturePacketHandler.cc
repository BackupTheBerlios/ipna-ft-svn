#include <iostream>
#include <stdio.h>
#include <sys/socket.h>

#include <ipna/network/Socket.hpp>
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

CapturePacketHandler::CapturePacketHandler(RecordWriter::RecordWriterPtr writer) :
  sequenceChecker(new SequenceNumberChecker()),
  parserFactory(ParserFactory::getInstance()),
  _recordWriter(writer)
{
  records = PacketParser::RecordVectorPtr(new PacketParser::RecordVector());
}

CapturePacketHandler::~CapturePacketHandler() {
}

bool
CapturePacketHandler::handlePacket(ipna::network::Packet::PacketPtr packet) {
  ParserPtr parser = parserFactory->getParser(packet->getBytes());

  // parser->analyze(packet);
  // records = parser->parse(packet);
  // writer->write(records)
  size_t numNewRecords = parser->parse(packet, records);
  _recordWriter->write(records);
  records->clear();
  
  struct cnfp_v9_hdr header;
  
  // analyze a little bit
  header = *(struct cnfp_v9_hdr*)packet->getBytes();
  //  checkSequenceNumber(ntohl(header.seq));
  SequenceNumberChecker::SequenceError seq_err =
    sequenceChecker->check(ntohl(header.seq));
  if (seq_err == SequenceNumberChecker::SEQ_MISSED) {
    LOG_WARN("missed " << (int)(sequenceChecker->missed()) << " packet(s)");
  }

  return true;
}
