/*
  FanoutPacketHandler.hpp - forwards packets to some other destinations.

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
#ifndef FANOUT_PACKET_HANDLER_H
#define FANOUT_PACKET_HANDLER_H

#include <vector>

#include <ipna/network/HostPort.hpp>
#include <ipna/network/PacketHandler.hpp>
#include <ipna/network/Packet.hpp>
#include <ipna/network/HostPort.hpp>
#include <ipna/Logger.hpp>

#include <QUdpSocket>
#include <QHostAddress>

namespace ipna {
  namespace network {
    class SequenceNumberChecker;
  }
  
  namespace fanout {
    class FanoutPacketHandler : public ipna::network::PacketHandler {
    public:
      FanoutPacketHandler(boost::shared_ptr<QUdpSocket> s);
      virtual ~FanoutPacketHandler();
      virtual bool handlePacket(network::Packet::PacketPtr packet);
      virtual FanoutPacketHandler* addDestination(const network::HostPort& hp);
      virtual FanoutPacketHandler* addEngineMapping(const network::HostPort& hp);
    protected:
      virtual bool setEngineId(network::Packet::PacketPtr packet);
    private:
      static ipna::Logger::LoggerPtr logger;
      boost::shared_ptr<QUdpSocket> socket;
      boost::shared_ptr<ipna::network::SequenceNumberChecker> sequenceChecker;
      std::vector<network::HostPort> destinations;
      std::vector<network::HostPort> engineMapping;
    };
  }
}

#endif // FANOUT_PACKET_HANDLER_H
