#ifndef FANOUT_PACKET_HANDLER_H
#define FANOUT_PACKET_HANDLER_H

#include <boost/shared_ptr.hpp>
#include <vector>

#include "PacketHandler.hpp"
#include "Logger.hpp"

namespace ipna {
  class Socket;
  class SequenceNumberChecker;
  
  class FanoutPacketHandler : public PacketHandler {
  public:
    typedef boost::shared_ptr<struct sockaddr_in> DestinationPtr;
    typedef std::vector<DestinationPtr>::iterator DestinationIterator;
    
    FanoutPacketHandler(boost::shared_ptr<Socket> s);
    virtual ~FanoutPacketHandler();
    virtual bool handlePacket(Packet packet, int len, struct sockaddr_in & from);
    virtual FanoutPacketHandler* addDestination(DestinationPtr d);
  private:
    void checkSequenceNumber(unsigned int s);
    
    static Logger::LoggerPtr logger;
    boost::shared_ptr<Socket> socket;
    boost::shared_ptr<SequenceNumberChecker> sequenceChecker;
    std::vector<DestinationPtr> destinations;
    
    unsigned int lastSequenceIdx;
    unsigned int SEQLEN;
    unsigned int* sequenceNumber;
  };
}

#endif // FANOUT_PACKET_HANDLER_H
