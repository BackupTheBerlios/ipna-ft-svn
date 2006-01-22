#ifndef PACKET_HPP
#define PACKET_HPP

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

#include <ipna/network/HostAddress.hpp>

namespace ipna {
  namespace network {
    class Packet {
      
    public:
      typedef boost::shared_array<char> PacketData;
      typedef boost::shared_ptr<Packet> PacketPtr;
      
      Packet(PacketData pd, size_t dataLen, const HostAddress& from);
      virtual ~Packet();

      inline size_t getLength() const { return _length; }
      const char* const getBytes(int startPosition=0) const;
      inline const HostAddress& getFrom() const { return _from; }
    private:
      size_t _length;
      unsigned int _currentPosition;
      HostAddress _from;
      PacketData _data;
    };
  }
}

#endif // PACKET_HPP
