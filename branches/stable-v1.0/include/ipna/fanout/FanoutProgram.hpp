#ifndef CAPTURE_PROGRAM_HPP
#define CAPTURE_PROGRAM_HPP

#include <string>

#include <boost/shared_ptr.hpp>

#include <ipna/IPNAProgram.hpp>
#include <ipna/network/Listener.hpp>
#include <ipna/fanout/FanoutPacketHandler.hpp>

namespace ipna {
  namespace fanout {
    class FanoutProgram : public ipna::IPNAProgram {
    public:
      FanoutProgram(const std::string& name = "ipna-ft-capture");
      virtual ~FanoutProgram() {}

      virtual void initialize(int argc, char **argv);
      virtual void start();
      virtual void stop() { if (_listener.get()) _listener->stop(); }
    private:
      boost::shared_ptr<network::Listener> _listener;
      boost::shared_ptr<fanout::FanoutPacketHandler> _handler;
    };
  }
}

#endif
