#include <ipna/network/Packet.hpp>

using namespace ipna;
using namespace ipna::network;

Packet::Packet(Packet::PacketData pd, size_t len, const HostAddress& from)
  : _data(pd), _length(len), _from(from), _currentPosition(0) {
}

Packet::~Packet() {}

const char* const
Packet::getBytes(int startPosition) const {
  if (startPosition < _length) {
    return _data.get() + startPosition;
  } else {
    return NULL;
  }
}

unsigned short
Packet::getNextShort() {
  unsigned short s = ntohs(*(unsigned short*)getCurrentBytes());
  _currentPosition += sizeof(unsigned short);
  return s;
}

unsigned int
Packet::getNextInt() {
  unsigned int i = ntohl(*(unsigned int*)getCurrentBytes());
  _currentPosition += sizeof(unsigned int);
  return i;
}  
