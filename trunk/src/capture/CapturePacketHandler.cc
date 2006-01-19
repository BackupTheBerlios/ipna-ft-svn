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
  sequenceChecker(new SequenceNumberChecker()) {
  parserFactory = ParserFactory::getInstance();
}

CapturePacketHandler::~CapturePacketHandler() {
}

bool
CapturePacketHandler::handlePacket(boost::shared_array<char> packet, int len, struct sockaddr_in & from) {
  ParserPtr parser = parserFactory->getParser(packet);

  // parser->analyze(packet);
  // records = parser->parse(packet);
  // writer->write(records)
  
  struct cnfp_v9_hdr header;
  
  // analyze a little bit
  header = *(struct cnfp_v9_hdr*)packet.get();
  //  checkSequenceNumber(ntohl(header.seq));
  SequenceNumberChecker::SequenceError seq_err =
    sequenceChecker->check(ntohl(header.seq));
  if (seq_err == SequenceNumberChecker::SEQ_MISSED) {
    LOG_WARN("missed " << (int)(sequenceChecker->missed()) << " packet(s)");
  }

  if (logger->isDebugEnabled()) {
    fprintf(stdout, "version:%d count:%u uptime:%u tstamp:%u seq:%u source:%d\n",
	    ntohs(header.common.version),
	    ntohs(header.common.count),
	    ntohl(header.uptime),
	    ntohl(header.tstamp),
	    ntohl(header.seq),
	    ntohl(header.engine_id)
	    );
  }

  return true;
}
