#ifndef LISTENER_H
#define LISTENER_H

#include <vector>
#include <boost/shared_ptr.hpp>
#include <ipna/Logger.hpp>
#include <ipna/network/Packet.hpp>
#include <QUdpSocket>

namespace ipna {
  namespace network {
    class PacketHandler;

    class Listener {
    public:
      typedef boost::shared_ptr<PacketHandler> HandlerPtr;
      typedef std::vector<HandlerPtr>::iterator HandlerIterator;
      
      Listener(boost::shared_ptr<QUdpSocket> s, unsigned int maxpacketlen = 2048);
      virtual ~Listener();
      virtual void start();
      virtual Listener* addHandler(HandlerPtr h);
    private:
      boost::shared_ptr<QUdpSocket> _socket;
      std::vector<HandlerPtr> handler;
      Packet::PacketData packetData;
      unsigned int maxpacketlen;
      static Logger::LoggerPtr logger;
    };
  }
}
#endif // LISTENER_H
