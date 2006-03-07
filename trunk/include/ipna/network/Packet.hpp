/*
  Packet.hpp - a packet.

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
#ifndef PACKET_HPP
#define PACKET_HPP

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <vector>

#include <QHostAddress>

namespace ipna {
  namespace network {
    class Packet {
      
    public:
      typedef boost::shared_array<char> PacketData;
      typedef boost::shared_ptr<Packet> PacketPtr;

      typedef struct frame_info_s {
	size_t start;
	size_t end;
	size_t pos;
      } frame_info;

      Packet(PacketData pd, size_t dataLen, const QHostAddress& from);
      virtual ~Packet() { _frames.clear(); }

      inline size_t getLength() const { return _length; }
      size_t startFrame(size_t length);
      size_t endFrame();
      const char* const getBytes(int startPosition = 0) const;
      const char* const getCurrentBytes() const {
	return getBytes(getCurrentPosition());
      }
      inline size_t getCurrentPosition() const { return getConstFrame().pos; }
      PacketData extractBytes(size_t startPosition, size_t length) const;
      
      unsigned short getNextShort();
      unsigned int getNextInt();
      bool advanceBytes(size_t numBytes);
      bool skipFrame();
      bool moveCursor(int offset);
      size_t dataLeft() const;
      size_t dataLeftInFrame() const;
      size_t numRemainingBytesInFrame() const;
      inline const QHostAddress& getFrom() const { return _from; }
    protected:
      const frame_info& getConstFrame(int idx = -1) const;
      frame_info& getFrame(int idx = -1);
      frame_info  popFrame();
      void pushFrame(size_t start, size_t end);
    private:
      std::vector<frame_info> _frames;
      size_t _length;
      QHostAddress _from;
      PacketData _data;
    };
  }
}

#endif // PACKET_HPP
