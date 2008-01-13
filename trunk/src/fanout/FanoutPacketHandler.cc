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
  struct cnfp_common_hdr *common_hdr = (struct cnfp_common_hdr*)(packet->getBytes());

  // handle version
  switch (ntohs(common_hdr->version)) {
	case CNFPv5:
	        {
                struct cnfp_v5_hdr *header = (struct cnfp_v5_hdr*)packet->getBytes();
                typedef vector<network::HostPort> EngineMap;
                for (EngineMap::iterator it = engineMapping.begin(); it != engineMapping.end(); it++) {
                  if (packet->getFrom() == it->host) {
                    LOG_DEBUG("mapping " << packet->getFrom().toString().toStdString() << " to " << it->port);
                    header->engine_id = htonl(it->port);
                    return true;
                  }
                }
		}
		break;
	case CNFPv9:
	        {
                struct cnfp_v9_hdr *header = (struct cnfp_v9_hdr*)packet->getBytes();
                typedef vector<network::HostPort> EngineMap;
                for (EngineMap::iterator it = engineMapping.begin(); it != engineMapping.end(); it++) {
                  if (packet->getFrom() == it->host) {
                    LOG_DEBUG("mapping " << packet->getFrom().toString().toStdString() << " to " << it->port);
                    header->engine_id = htonl(it->port);
                    return true;
                  }
                }
		}
		break;
	default:
		LOG_ERROR("setEngineID(): netflow format " << common_hdr->version << " currently not supported!");
		break;
  }

  return false;
}

bool
FanoutPacketHandler::handlePacket(Packet::PacketPtr packet) {
  setEngineId(packet);
  
  unsigned short version = 0;
  unsigned short count = 0;
  unsigned int uptime = 0;
  unsigned int tstamp = 0;
  unsigned int seq = 0;
  int engine_id = -1;
  unsigned int __seq_increment = 1;

  struct cnfp_common_hdr *common_hdr = (struct cnfp_common_hdr*)(packet->getBytes());
  switch (ntohs(common_hdr->version)) {
  	case CNFPv5:
		{
		  // analyze a little bit
		  struct cnfp_v5_hdr header = *(struct cnfp_v5_hdr*)packet->getBytes();
		  version = ntohs(header.common.version);
		  count = ntohs(header.common.count);
		  uptime = ntohl(header.uptime);
		  tstamp = ntohl(header.tstamp);
		  seq = ntohl(header.seq);
		  engine_id = ntohl(header.engine_id);
		  __seq_increment = count;
		}
		break;
	case CNFPv9:
		{
		  // analyze a little bit
		  struct cnfp_v9_hdr header = *(struct cnfp_v9_hdr*)packet->getBytes();
		  version = ntohs(common_hdr->version);
		  count = ntohs(header.common.count);
		  uptime = ntohl(header.uptime);
		  tstamp = ntohl(header.tstamp);
		  seq = ntohl(header.seq);
		  engine_id = ntohl(header.engine_id);
		  __seq_increment = 1;
		}
		break;
	default:
		LOG_ERROR("handlePacket(): netflow format " << common_hdr->version << " currently not supported!");
		break;
  }

  SequenceNumberChecker::SequenceError seq_err =
    sequenceChecker->check(seq, __seq_increment);
  if (seq_err == SequenceNumberChecker::SEQ_MISSED) {
    LOG_DEBUG("missed " << (int)(sequenceChecker->missed()) << " packet(s)");
  }

  if (logger->isDebugEnabled()) {
    fprintf(stderr, "version:%d count:%u uptime:%u tstamp:%u seq:%u source:%d\n",
	    version, count, uptime, tstamp, seq, engine_id);
  } 

  bool deliveredAll = true;
  // deliver packets
  for (vector<network::HostPort>::const_iterator d = destinations.begin(); d != destinations.end(); d++) {
    qint64 sent = socket->writeDatagram(packet->getBytes(), packet->getLength(), d->host, d->port);
    if (sent == -1) {
      LOG_WARN("could not deliver datagram: " << socket->errorString().toStdString());
      deliveredAll = false;
    }
  }
  return deliveredAll;
}
