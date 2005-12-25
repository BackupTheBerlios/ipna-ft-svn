#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include <iostream>

#include <boost/shared_ptr.hpp>

#include "Listener.h"
#include "PacketHandler.h"
#include "Socket.h"
#include "Logger.hpp"

using namespace std;
using namespace ipna;

Logger::LoggerPtr Listener::logger = Logger::getLogger("ipna.ft.listener");

Listener::Listener(boost::shared_ptr<Socket> s, unsigned int maxpacketlen)
  : socket(s), maxpacketlen(maxpacketlen) {
  packet = boost::shared_array<char>(new char[maxpacketlen]);
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
    struct sockaddr_in from;
    size_t fromlen = sizeof(struct sockaddr);
    memset(packet.get(), 0, maxpacketlen);
    int received = socket->recvfrom(packet.get(), maxpacketlen, (struct sockaddr*)&from, &fromlen);
    if (received < 0) {
      // could not receive packet, so return
      perror("recvfrom");
      return;
    } else {
      time_t duration = time(NULL);
      for (HandlerIterator h = handler.begin(); h != handler.end(); h++) {
	if (!h->get()->handlePacket(packet, received)) {
	  // could not handle packet, so return
	  return;
	}
      }
      duration = time(NULL) - duration;
    }
  }
}
