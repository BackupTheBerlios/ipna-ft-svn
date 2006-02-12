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
    private:
      boost::shared_ptr<network::Listener> _listener;
      boost::shared_ptr<capture::Formatter> _formatter;
      boost::shared_ptr<capture::RecordWriter> _writer;
      boost::shared_ptr<capture::CapturePacketHandler> _handler;
    };
  }
}

#endif
