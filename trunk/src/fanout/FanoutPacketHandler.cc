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
#include <arpa/inet.h>

#include <QHostAddress>

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

FanoutPacketHandler*
FanoutPacketHandler::addEngineMapping(const HostPort& hp) {
  engineMapping.push_back(hp);
  return this;
}

bool
FanoutPacketHandler::setEngineId(Packet::PacketPtr packet) {
  struct cnfp_v9_hdr *header = (struct cnfp_v9_hdr*)packet->getBytes();
  typedef vector<network::HostPort> EngineMap;
  for (EngineMap::iterator it = engineMapping.begin(); it != engineMapping.end(); it++) {
    if (packet->getFrom() == it->host) {
      LOG_DEBUG("mapping " << packet->getFrom().toString().toStdString() << " to " << it->port);
      header->engine_id = htonl(it->port);
      return true;
    }
  }
  return false;
}

bool
FanoutPacketHandler::handlePacket(Packet::PacketPtr packet) {
  setEngineId(packet);
  
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
