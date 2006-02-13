#include <ipna/network/Packet.hpp>

#include <arpa/inet.h>
#include <algorithm>

using namespace ipna;
using namespace ipna::network;

Packet::Packet(Packet::PacketData pd, size_t len, const QHostAddress& from)
  : _data(pd), _length(len), _from(from) {
  pushFrame(0, _length);
}

void
Packet::pushFrame(size_t start, size_t end) {
  frame_info frame;
  frame.start = start;
  frame.end   = end;
  frame.pos   = start;
  _frames.push_back(frame);
}

Packet::frame_info
Packet::popFrame() {
  assert(_frames.size() > 1);
  frame_info f = _frames.back();
  _frames.pop_back();
  return f;
}


Packet::frame_info&
Packet::getFrame(int index) {
  if (index < 0) {
    index = _frames.size() + index;
  }
  assert(0 <= index && (size_t)index < _frames.size());
  return _frames[(size_t)index];
}

const Packet::frame_info&
Packet::getConstFrame(int index) const {
  if (index < 0) {
    index = _frames.size() + index;
  }
  assert(0 <= index && (size_t)index < _frames.size());
  return _frames[(size_t)index];
}

size_t
Packet::startFrame(size_t length) {
  const frame_info cur = getConstFrame();
  size_t start = cur.pos;
  size_t end = std::min(start + length, getLength());
  pushFrame(start,end);
  return end;
}

size_t
Packet::endFrame() {
  if (_frames.size() > 1) {
    frame_info old = popFrame();
    frame_info& cur = getFrame();
    cur.pos = old.pos;
    if (cur.pos > cur.end)
      endFrame();
  }
}

bool
Packet::skipFrame() {
  frame_info& cur = getFrame();
  cur.pos = cur.end;
  endFrame();
  /*  size_t skippedBytes = cur.end - cur.pos;
  cur.pos = cur.end + 1;
  endFrame();
  */
  return true; //skippedBytes > 0;
}

bool
Packet::moveCursor(int offset) {
  frame_info& cur = getFrame();
  cur.pos += offset;
  assert(cur.pos >= cur.start);
  assert(cur.pos <= cur.end);
  assert(cur.pos <= getLength());
  return cur.pos <= getLength();
}

bool
Packet::advanceBytes(size_t num) {
  return moveCursor(num);
}

const char* const
Packet::getBytes(int startPosition) const {
  if (startPosition < _length) {
    return _data.get() + startPosition;
  } else {
    return NULL;
  }
}

Packet::PacketData
Packet::extractBytes(size_t startPosition, size_t length) const {
  if (0 == length || (startPosition + length) > _length) {
    return Packet::PacketData(NULL);
  } else {
    char* foo = new char[length];
    if (foo != NULL)
      memcpy(foo, getBytes(startPosition), length);
    Packet::PacketData pd(foo);
    return pd;
  }
}

size_t
Packet::dataLeft() const {
  if (getCurrentPosition() < getLength()) {
    return getLength() - getCurrentPosition();
  } else {
    return 0;
  }
}

size_t
Packet::dataLeftInFrame() const {
  const frame_info cur = getConstFrame();
  if (cur.pos < cur.end) {
    return (cur.end - cur.pos);
  } else {
    return 0;
  }
}

unsigned short
Packet::getNextShort() {
  unsigned short s = ntohs(*(unsigned short*)getCurrentBytes());
  moveCursor(sizeof(unsigned short));
  const frame_info cur = getConstFrame();
  assert(cur.pos <= cur.end);
  return s;
}

unsigned int
Packet::getNextInt() {
  unsigned int i = ntohl(*(unsigned int*)getCurrentBytes());
  moveCursor(sizeof(unsigned short));
  const frame_info cur = getConstFrame();
  assert(cur.pos <= cur.end);
  return i;
}  
