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

CapturePacketHandler::CapturePacketHandler() :
  sequenceChecker(new SequenceNumberChecker()), parserFactory(ParserFactory::getInstance()) {
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
  for (PacketParser::RecordVector::iterator it = records->begin(); it != records->end(); it++) {
    cout << (*it)->get(10)->asUInt() << '\t';
    cout << (*it)->get(8)->asIp()    << '\t';
    cout << (*it)->get(14)->asUInt() << '\t';
    cout << (*it)->get(12)->asIp()   << '\t';
    cout << (*it)->get(4)->asUInt()  << '\t';
    cout << (*it)->get(7)->asUInt()  << '\t';
    cout << (*it)->get(11)->asUInt() << '\t';
    cout << (*it)->get(2)->asUInt()  << '\t';
    cout << (*it)->get(1)->asUInt()  << '\t';
    cout << endl;
  }
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
