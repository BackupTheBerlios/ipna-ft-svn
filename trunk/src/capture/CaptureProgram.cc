#include <ipna/capture/CaptureProgram.hpp>

#include <QUdpSocket>
#include <iostream>

#include <ipna/network/HostPort.hpp>

using namespace ipna;
using namespace ipna::capture;

CaptureProgram::CaptureProgram(const std::string& name)
  : IPNAProgram(name) {
  try {
    getOptions().add_options()
      ("only-from,o", po::value<std::string>(), "fan out packets only from this source ip")
      ("listen,l", po::value<std::string>(), "listen on ip/port")
      ;
  } catch (po::error & ex) {
    LOG_ERROR(ex.what());
    exit(1);
  }
}

void
CaptureProgram::initialize(int argc, char **argv) {
  IPNAProgram::initialize(argc,argv);

  if (!isSet(("listen"))) {
    LOG_ERROR("no ip/port pair specified to listen on!");
    std::cerr << getOptions() << std::endl;
    exit(1);
  }

  network::HostPort listenOn(getArgumentMap()["listen"].as<std::string>(), isSet("ipv6"));
  LOG_DEBUG("listening on: [" << listenOn.host.toString().toStdString() << "]:" << listenOn.port);

  typedef boost::shared_ptr<QUdpSocket> SocketPtr;
  SocketPtr listenSocket;
  listenSocket = SocketPtr(new QUdpSocket());
  if(! listenSocket->bind(listenOn.host, listenOn.port)) {
    LOG_ERROR("could not bind to listen-socket: " << listenSocket->errorString().toStdString());
    exit(3);
  }

  _listener  = boost::shared_ptr<network::Listener>(new network::Listener(listenSocket));
  _formatter = boost::shared_ptr<capture::Formatter>(new capture::Formatter());
  _writer    = boost::shared_ptr<capture::RecordWriter>(new capture::RecordWriter(_formatter ,std::cout));
  _handler   = boost::shared_ptr<capture::CapturePacketHandler>(new capture::CapturePacketHandler(_writer));
  _listener->addHandler(_handler);
}

void
CaptureProgram::start() {
  _listener->start();
}

