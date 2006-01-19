#ifndef PACKET_HPP
#define PACKET_HPP

#include <boost/shared_array.hpp>

namespace ipna {
  namespace network {
    class Packet {
      using boost::shared_array;
      
    public:
      typedef shared_array<char> PacketData;
      
      Packet(PacketData pd, size_t dataLen, struct sockaddr_in& from);
      virtual ~Packet();
      
      char  nextByte();
      short nextShort();
      int   nextInt();
      long  nextLong();
      
      boost::any next(size_t numBytes);
      
      PacketData getData();
    private:
      size_t _length;
      unsigned int _currentPosition;
      struct sockaddr_in _from;
      PacketData _data;
    };
  }
}

#endif // PACKET_HPP
