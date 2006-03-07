#include <iostream>
#include <stdio.h>
#include <sys/socket.h>

#include <algorithm>

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
  LOG_DEBUG("got " << numNewRecords << " new records");
  if (records->size() > _queueSize) {
    sort(records->begin(), records->end(), Record::compare_lt);

    _recordWriter->write(records);
    records->clear();
  }
  
  SequenceNumberChecker::SequenceError seq_err =
    sequenceChecker->check(seq);
  if (seq_err == SequenceNumberChecker::SEQ_MISSED) {
    LOG_WARN("missed " << (int)(sequenceChecker->missed()) << " packet(s)");
  }

  return true;
}
