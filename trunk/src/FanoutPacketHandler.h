#ifndef FANOUT_PACKET_HANDLER_H
#define FANOUT_PACKET_HANDLER_H

#include <boost/shared_ptr.hpp>
#include <vector>

#include "PacketHandler.h"

class Socket;

class FanoutPacketHandler : public PacketHandler {
 public:
  typedef boost::shared_ptr<struct sockaddr_in> DestinationPtr;
  typedef std::vector<DestinationPtr>::iterator DestinationIterator;
  
  FanoutPacketHandler(boost::shared_ptr<Socket> s);
  virtual ~FanoutPacketHandler();
  virtual bool handlePacket(Packet packet, int len);
  virtual FanoutPacketHandler* addDestination(DestinationPtr d);
 private:
  boost::shared_ptr<Socket> socket;
  std::vector<DestinationPtr> destinations;
  
  unsigned int lastSequenceIdx;
  unsigned int SEQLEN;
  unsigned int* sequenceNumber;
};

#endif // FANOUT_PACKET_HANDLER_H
