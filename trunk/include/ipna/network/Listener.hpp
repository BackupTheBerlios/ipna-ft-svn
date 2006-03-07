/*
  Listener.hpp - an object that listens on a socket lets a handler handle the packet.

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
      inline void stop() { _stopped = true; }
      inline bool isStopped() const { return _stopped; }
      virtual Listener* addHandler(HandlerPtr h);
    private:
      boost::shared_ptr<QUdpSocket> _socket;
      std::vector<HandlerPtr> handler;
      Packet::PacketData packetData;
      unsigned int maxpacketlen;
      static Logger::LoggerPtr logger;
      bool _stopped;
    };
  }
}

#endif // LISTENER_H
