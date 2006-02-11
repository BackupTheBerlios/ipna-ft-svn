#ifndef PACKET_HPP
#define PACKET_HPP

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

#include <QHostAddress>

namespace ipna {
  namespace network {
    class Packet {
      
    public:
      typedef boost::shared_array<char> PacketData;
      typedef boost::shared_ptr<Packet> PacketPtr;
      
      Packet(PacketData pd, size_t dataLen, const QHostAddress& from);
      virtual ~Packet();

      inline size_t getLength() const { return _length; }
      inline size_t startFrame(size_t length, int offset=0) {
	_frameStart = _currentPosition + offset;
	_frameEnd   = _frameStart + length;
	// maybe set the end point to the minimum of remainingBytes and length
	return _frameEnd;
      }
      const char* const getBytes(int startPosition = 0) const;
      const char* const getCurrentBytes() const {
	return getBytes(getCurrentPosition());
      }
      inline size_t getCurrentPosition() const { return _currentPosition; }
      PacketData extractBytes(size_t startPosition, size_t length) const;
      
      unsigned short getNextShort();
      unsigned int getNextInt();

      inline bool advanceBytes(size_t numBytes) {
	if (_currentPosition + numBytes < _length) {
	  _currentPosition += numBytes;
	  return true;
	} else {
	  return false;
	}
      }

      inline bool skipFrame() {
	if (_currentPosition < _frameEnd) {
	  _currentPosition = _frameEnd;
	  return true;
	} else {
	  return false;
	}
      }
      
      inline bool dataLeft() const { return _currentPosition < (_length); }
      inline size_t numRemainingBytesInFrame() const {
	if (_currentPosition < _frameEnd) {
	  return (_frameEnd - _currentPosition);
	} else {
	  return 0;
	}
      }
      inline size_t numTotalRemainingBytes() const {
	if (dataLeft()) {
	  return (_length - _currentPosition);
	} else {
	  return 0;
	}
      }

      inline const QHostAddress& getFrom() const { return _from; }
    private:
      size_t _length;
      size_t _currentPosition;
      size_t _frameStart;
      size_t _frameEnd;
      QHostAddress _from;
      PacketData _data;
    };
  }
}

#endif // PACKET_HPP
