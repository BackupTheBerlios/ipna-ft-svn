#include <ipna/fanout/FanoutProgram.hpp>

#include <string>
#include <vector>
#include <QUdpSocket>
#include <iostream>

#include <ipna/network/HostPort.hpp>

using namespace ipna;
using namespace ipna::fanout;

FanoutProgram::FanoutProgram(const std::string& name)
  : IPNAProgram(name) {
  try {
    getOptions().add_options()
      ("only-from,o", po::value<std::string>(), "fan out packets only from this source ip")
      ("listen,l", po::value<std::string>(), "listen on ip/port")
      ("engine-map,m", po::value<std::vector<std::string> >(), "ip to engine mapping (ip/engine)")
      ("export,e", po::value< std::vector<std::string> >(), "export to these probes (ip/port)")
      ;
    getPositionalOptions().add("export", -1);
  } catch (po::error & ex) {
    LOG_ERROR(ex.what());
    exit(1);
  }
}

void
FanoutProgram::initialize(int argc, char **argv) {
  IPNAProgram::initialize(argc,argv);

  if (!isSet(("listen"))) {
    LOG_ERROR("no ip/port pair specified to listen on!");
    printHelp();
    exit(1);
  }

  if (!isSet("export")) {
    LOG_ERROR("no ip/port pair specified to export to!");
    printHelp();
    exit(2);
  }

  network::HostPort listenOn(getArgumentMap()["listen"].as<std::string>(), isSet("ipv6"));
  LOG_INFO("listening on: [" << listenOn.host.toString().toStdString() << "]:" << listenOn.port);
  // parse export pairs and check them
  std::vector<network::HostPort> exportTo;
  std::vector<std::string> exports = getArgumentMap()["export"].as< std::vector<std::string> >();
  for (std::vector<std::string>::iterator it = exports.begin(); it != exports.end(); it++) {
    try {
      exportTo.push_back(network::HostPort(*it, isSet("ipv6")));
    } catch (std::string& e) {
      LOG_ERROR(e);
    }
  }

  {
    std::stringstream ss;
    for (std::vector<network::HostPort>::iterator it = exportTo.begin(); it != exportTo.end(); it++) {
      ss << "[" << it->host.toString().toStdString() << "]:" << it->port << " ";
    }
    LOG_INFO("exporting to: " << ss.str());
  }

  typedef boost::shared_ptr<QUdpSocket> SocketPtr;

  SocketPtr listenSocket;
  SocketPtr sendSocket;
    
  listenSocket = SocketPtr(new QUdpSocket());
  if(! listenSocket->bind(listenOn.host, listenOn.port)) {
    LOG_ERROR("could not bind to listen-socket: " << listenSocket->errorString().toStdString());
    exit(3);
  }

  sendSocket = SocketPtr(new QUdpSocket());
  if (! sendSocket->bind()) {
    LOG_ERROR("could not bind to send-socket: " << sendSocket->errorString().toStdString());
    exit(3);
  }

  _listener  = boost::shared_ptr<network::Listener>(new network::Listener(listenSocket));
  _handler   = boost::shared_ptr<ipna::fanout::FanoutPacketHandler>(new ipna::fanout::FanoutPacketHandler(sendSocket));
  _listener->addHandler(_handler);
    
  // create destination infos:
  for (std::vector<network::HostPort>::iterator it = exportTo.begin(); it != exportTo.end(); it++) {
    _handler->addDestination(*it);
  }

  // create engine id mappings:
  std::vector<std::string> engineMappings = getArgumentMap()["engine-map"].as< std::vector<std::string> >();
  for (std::vector<std::string>::iterator it = engineMappings.begin(); it != engineMappings.end(); it++) {
    try {
      _handler->addEngineMapping(network::HostPort(*it,isSet("ipv6")));
    } catch (std::string& e) {
      LOG_ERROR(e);
    }
  }
}

void
FanoutProgram::start() {
  _listener->start();
}

