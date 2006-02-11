#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <ipna/network/HostPort.hpp>
#include <ipna/network/Listener.hpp>
#include <ipna/network/PacketHandler.hpp>
#include <ipna/fanout/FanoutPacketHandler.hpp>
#include <ipna/Logger.hpp>

#include <QString>
#include <QUdpSocket>
#include <QHostAddress>

namespace po = boost::program_options;
using namespace std;
using namespace ipna;


Logger::LoggerPtr logger;

int main(int argc, char** argv) {
  string pidfile;

  logger = Logger::getLogger("ipna");    

  // 1. parse options
  // 2. listen on specified source
  // 3. loop:
  //    send packets to specified destinations
  po::options_description desc("Allowed options");
  po::positional_options_description p;
  po::variables_map vm;

  try {
    desc.add_options()
      ("help,h", po::value< vector<bool> >()->zero_tokens(), "show this help message")
      ("pidfile,p", po::value<string>()->default_value("/tmp/ipna-ft-fanout.pid"), "use this file to store the pid")
      ("verbose,v", "verbose output")
      ("ipv4,4", "use ipv4")
      ("ipv6,6", "use ipv6, that is the default but currently not implemented!")
      ("only-from,o", po::value<string>(), "fan out packets only from this source ip")
      ("listen,l", po::value<string>(), "listen on ip/port")
      ("export,e", po::value< vector<string> >(), "export to these probes (ip/port)")
      ;
    p.add("export", -1);
  } catch (po::error & ex) {
    LOG_ERROR(ex.what());
    exit(1);
  }

  try {
    po::store(po::command_line_parser(argc,argv).options(desc).positional(p).run(), vm);
    po::notify(vm);
  } catch (po::error & ex) {
    LOG_ERROR(ex.what());
    cerr << desc << endl;
    exit(1);
  } catch (...) {
    LOG_ERROR("unknown things happened!");
    exit(1);
  }

  if (vm.count("verbose")) {
    Logger::getRootLogger()->setPriority(Logger::DEBUG);
  } else {
    Logger::getRootLogger()->setPriority(Logger::WARN);
  }

  if (vm.count("help")) {
    cerr << desc << endl;
    exit(0);
  }
    
  if (vm.count("pidfile")) {
    pidfile = vm["pidfile"].as<string>();
  }        

  if (0 == vm.count("listen")) {
    LOG_ERROR("no ip/port pair specified to listen on!");
    cerr << desc << endl;
    exit(1);
  }

  if (0 == vm.count("export")) {
    LOG_ERROR("no ip/port pair specified to export to!");
    cerr << desc << endl;
    exit(2);
  }

  LOG_DEBUG("using pidfile: \"" << pidfile << "\"");

  try {
    ofstream _pid_fstream(pidfile.c_str());
    if (!_pid_fstream.is_open()) {
      throw string("could not open file: ").append(pidfile.c_str());
    }
    _pid_fstream << getpid() << endl;
    _pid_fstream.close();
  } catch (string & ex) {
    LOG_ERROR("could not write pid: " + ex);
    exit(1);
  }

  network::HostPort listenOn(vm["listen"].as<string>(), vm.count("ipv6") > 0);
  LOG_DEBUG("listening on: [" << listenOn.host.toString().toStdString() << "]:" << listenOn.port);

  // parse export pairs and check them
  vector<network::HostPort> exportTo;
  vector<string> exports = vm["export"].as< vector<string> >();
  for (vector<string>::iterator it = exports.begin(); it != exports.end(); it++) {
    exportTo.push_back(network::HostPort(*it, vm.count("ipv6") > 0 ));
  }

  {
    stringstream ss;
    for (vector<network::HostPort>::iterator it = exportTo.begin(); it != exportTo.end(); it++) {
      ss << "[" << it->host.toString().toStdString() << "]:" << it->port << ", ";
    }
    LOG_DEBUG("exporting to: " << ss.str());
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

  boost::shared_ptr<ipna::fanout::FanoutPacketHandler> fanout(new ipna::fanout::FanoutPacketHandler(sendSocket));
  boost::shared_ptr<network::Listener> listener(new network::Listener(listenSocket));
  listener->addHandler(fanout);
    
  // create destination infos:
  for (vector<network::HostPort>::iterator it = exportTo.begin(); it != exportTo.end(); it++) {
    fanout->addDestination(*it);
  }

  listener->start();
}
