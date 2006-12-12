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
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>

#include <boost/timer.hpp>
#include <boost/shared_ptr.hpp>

#include <ipna/network/Listener.hpp>
#include <ipna/network/PacketHandler.hpp>
#include <ipna/Logger.hpp>

#include <QUdpSocket>
#include <QHostAddress>

using namespace std;
using namespace ipna;
using namespace ipna::network;

Logger::LoggerPtr Listener::logger = Logger::getLogger("ipna.ft.listener");

Listener::Listener(boost::shared_ptr<QUdpSocket> s, unsigned int maxpacketlen)
  : _socket(s), maxpacketlen(maxpacketlen), _stopped(false) {
  packetData = Packet::PacketData(new char[maxpacketlen]);
  _socket->blockSignals(true);
}

Listener::~Listener() {
}

Listener*
Listener::addHandler(HandlerPtr h) {
  handler.push_back(h);
  return this;
}

void
Listener::start() {
  _stopped = false;
  
  while (!isStopped()) {
    if (!_socket->hasPendingDatagrams()) {
      for (HandlerIterator h = handler.begin(); h != handler.end(); h++) {
	h->get()->handleTimeout();
      }

      usleep(500);
      continue;
    }

    qint64 dgsize = _socket->pendingDatagramSize();
    if (dgsize > maxpacketlen) {
      packetData = Packet::PacketData(new char[dgsize]);
      maxpacketlen = dgsize;
    }
    memset(packetData.get(), 0, maxpacketlen);

    QHostAddress fromAddr;
    quint16 fromPort;
    qint64 received = _socket->readDatagram(packetData.get(), dgsize, &fromAddr, &fromPort);
    if (received < 0) {
      LOG_WARN("could not receive datagram: " << _socket->error() << ": " << _socket->errorString().toStdString() );
      continue;
    }

    struct timeval tstart, tfinish;
    double tsecs;

    Packet::PacketPtr packet(new Packet(packetData, received, fromAddr));

    gettimeofday(&tstart, NULL);
    for (HandlerIterator h = handler.begin(); h != handler.end(); h++) {
      if (!h->get()->handlePacket(packet)) {
	// could not handle packet, so return
	LOG_WARN("could not handle packet!");
	return;
      }
    }

    gettimeofday(&tfinish, NULL);
    tsecs = (tfinish.tv_sec - tstart.tv_sec) +
      1e-6 * (tfinish.tv_usec - tstart.tv_usec);
    if (tsecs > 0.4) {
      LOG_WARN("processing took " << tsecs << "s");
    } else {
      LOG_DEBUG("processing took " << tsecs << "s");
    }	
  }
}
