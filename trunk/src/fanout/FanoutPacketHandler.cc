#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <ipna/fanout/FanoutPacketHandler.hpp>
#include <ipna/network/SequenceNumberChecker.hpp>
#include <ipna/parser/cnfp.hpp>

using namespace std;
using namespace ipna;
using namespace ipna::fanout;
using namespace ipna::network;
using namespace ipna::parser;

Logger::LoggerPtr FanoutPacketHandler::logger = Logger::getLogger("ipna.fanout");

FanoutPacketHandler::FanoutPacketHandler(boost::shared_ptr<QUdpSocket> s) :
  socket(s), sequenceChecker(new SequenceNumberChecker()) {
}

FanoutPacketHandler::~FanoutPacketHandler() {
  destinations.clear();
}

FanoutPacketHandler*
FanoutPacketHandler::addDestination(const network::HostPort& hp) {
  destinations.push_back(hp);
  return this;
}

bool
FanoutPacketHandler::handlePacket(Packet::PacketPtr packet) {
  struct cnfp_v9_hdr header;
  
  // analyze a little bit
  header = *(struct cnfp_v9_hdr*)packet->getBytes();
  //  checkSequenceNumber(ntohl(header.seq));
  SequenceNumberChecker::SequenceError seq_err =
    sequenceChecker->check(ntohl(header.seq));
  if (seq_err == SequenceNumberChecker::SEQ_MISSED) {
    LOG_WARN("missed " << (int)(sequenceChecker->missed()) << " packet(s)");
  }

  if (logger->isDebugEnabled()) {
    fprintf(stderr, "version:%d count:%u uptime:%u tstamp:%u seq:%u source:%d\n",
	    ntohs(header.common.version),
	    ntohs(header.common.count),
	    ntohl(header.uptime),
	    ntohl(header.tstamp),
	    ntohl(header.seq),
	    ntohl(header.engine_id)
	    );
  }

  bool deliveredAll = true;
  // deliver packets
  for (vector<network::HostPort>::const_iterator d = destinations.begin(); d != destinations.end(); d++) {
    quint64 sent = socket->writeDatagram(packet->getBytes(), packet->getLength(), d->host, d->port);
    if (sent == -1) {
      LOG_WARN("could not deliver datagram: " << socket->errorString().toStdString());
      deliveredAll = false;
    }
  }
  return deliveredAll;
}
