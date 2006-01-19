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
  socket(s), lastSequenceIdx(0), SEQLEN(32), sequenceChecker(new SequenceNumberChecker()) {
  sequenceNumber = new unsigned int[SEQLEN];
  memset(sequenceNumber, 0, SEQLEN * sizeof(unsigned int));
  //  logger = Logger::getLogger("ipna.fanout");  
}

FanoutPacketHandler::~FanoutPacketHandler() {
  destinations.clear();
  delete [] sequenceNumber;
  sequenceNumber = NULL;
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

void
FanoutPacketHandler::checkSequenceNumber(unsigned int newSequence) {
  unsigned int foundIdx = SEQLEN;
  // look if we had the sequence number - 1 in our window... worst-case: O(n) best-case: O(1)
  for (unsigned int i = lastSequenceIdx; i != (lastSequenceIdx-1)%SEQLEN; i = (i+1)%SEQLEN) {
    if ((sequenceNumber[i]+1) == newSequence) {
      foundIdx = i;
      break;
    } else if ((sequenceNumber[i] > newSequence)) {
      foundIdx = i;
      break;
    }
  }
  
  if (foundIdx != SEQLEN && ((sequenceNumber[lastSequenceIdx]+1) != newSequence)) {
    LOG_WARN("packet-reordering occured within window: [" << sequenceNumber[foundIdx] << ":" << sequenceNumber[lastSequenceIdx] << "]");
  } else if (foundIdx == SEQLEN) {
    LOG_WARN("missed " << (int)(newSequence - sequenceNumber[lastSequenceIdx]) << " packet(s)");
  }

  // store the new sequence
  lastSequenceIdx = (lastSequenceIdx+1)%SEQLEN;
  sequenceNumber[lastSequenceIdx] = newSequence;
}
