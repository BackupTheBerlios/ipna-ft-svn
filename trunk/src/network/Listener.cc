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
  : _socket(s), maxpacketlen(maxpacketlen) {
  packetData = Packet::PacketData(new char[maxpacketlen]);
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
  for (;;) {
    if (!_socket->hasPendingDatagrams()) {
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
      LOG_WARN("could not receive datagram");
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
