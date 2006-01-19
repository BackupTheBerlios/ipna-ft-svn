#include <iostream>
#include <stdio.h>
#include <sys/socket.h>

#include <ipna/network/Socket.hpp>
#include <ipna/fanout/FanoutPacketHandler.hpp>
#include <ipna/network/SequenceNumberChecker.hpp>
#include <ipna/parser/cnfp.hpp>

using namespace std;
using namespace ipna;
using namespace ipna::fanout;
using namespace ipna::network;
using namespace ipna::parser;

Logger::LoggerPtr FanoutPacketHandler::logger = Logger::getLogger("ipna.fanout");

FanoutPacketHandler::FanoutPacketHandler(boost::shared_ptr<Socket> s) :
  socket(s), sequenceChecker(new SequenceNumberChecker()) {
}

FanoutPacketHandler::~FanoutPacketHandler() {
  destinations.clear();
}

FanoutPacketHandler*
FanoutPacketHandler::addDestination(DestinationPtr d) {
  destinations.push_back(d);
  return this;
}

bool
FanoutPacketHandler::handlePacket(boost::shared_array<char> packet, int len, struct sockaddr_in & from) {
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

  // deliver packets
  for (DestinationIterator d = destinations.begin(); d != destinations.end(); d++) {
    int sent = socket->sendto(packet.get(), len, (struct sockaddr*)(d->get()));
    if (sent == -1) {
      perror("sendto");
      return false;
    }  
  }
}
