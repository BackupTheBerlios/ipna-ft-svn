#ifndef LISTENER_H
#define LISTENER_H

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

class PacketHandler;
class Socket;

class Listener {
 public:
  typedef boost::shared_ptr<PacketHandler> HandlerPtr;
  typedef std::vector<HandlerPtr>::iterator HandlerIterator;
  
  Listener(boost::shared_ptr<Socket> s, unsigned int maxpacketlen = 2048);
  virtual ~Listener();
  virtual void start();
  virtual Listener* addHandler(HandlerPtr h);
 private:
  boost::shared_ptr<Socket> socket;
  std::vector<HandlerPtr> handler;
  boost::shared_array<char> packet;
  unsigned int maxpacketlen;
};

#endif // LISTENER_H
