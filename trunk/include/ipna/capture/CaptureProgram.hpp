/*
  CaptureProgram.hpp - capture program.

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
#ifndef CAPTURE_PROGRAM_HPP
#define CAPTURE_PROGRAM_HPP

#include <string>

#include <boost/shared_ptr.hpp>

#include <ipna/IPNAProgram.hpp>
#include <ipna/network/Listener.hpp>
#include <ipna/network/PacketHandler.hpp>
#include <ipna/capture/RecordWriter.hpp>
#include <ipna/capture/CapturePacketHandler.hpp>

namespace ipna {
  namespace capture {
    class CaptureProgram : public ipna::IPNAProgram {
    public:
      CaptureProgram(const std::string& name = "ipna-ft-capture");
      virtual ~CaptureProgram() {}

      virtual void initialize(int argc, char **argv);
      virtual void start();
      virtual void stop() { if (_listener.get()) _listener->stop(); }
    private:
      boost::shared_ptr<network::Listener> _listener;
      boost::shared_ptr<capture::Formatter> _formatter;
      boost::shared_ptr<capture::RecordWriter> _writer;
      boost::shared_ptr<capture::CapturePacketHandler> _handler;
    };
  }
}

#endif
