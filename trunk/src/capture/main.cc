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

#include <ipna/network/Socket.hpp>
#include <ipna/network/Listener.hpp>
#include <ipna/network/PacketHandler.hpp>
#include <ipna/capture/CapturePacketHandler.hpp>
#include <ipna/Logger.hpp>

namespace po = boost::program_options;
using namespace std;
using namespace ipna;

typedef int Port;
typedef string Ip;
typedef pair<Ip, Port> IpPortPair;

Logger::LoggerPtr logger;

bool checkIpPortPair(IpPortPair& p) {
  Ip ip = p.first;
  Port port = p.second;

  // check port
  if (port < 0 || port > 65535) {
    return false;
  }

  struct hostent *hostCheck = gethostbyname(p.first.c_str());
  if (p.first != "*" && NULL == hostCheck) {
    herror("gethostbyname");
    return false;
  }
    
  return true;
}

template<class T>
T parseString(const string& s) {
  stringstream strm(s);
  T val;
  strm >> val;
  return val;
}

IpPortPair
splitIntoIpPortPair(const string& s) {
  // parse the pairs out of s
  IpPortPair p;

  vector<string> splitted;
  boost::split(splitted, s, boost::algorithm::is_any_of("/"));

  if (splitted.size() != 2) {
    LOG_ERROR("invalid ip:port! " << s);
    exit(1);
  }

  p.first = splitted[0];
  p.second = parseString<int>(splitted[1]);

  return p;
}

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
      ("pidfile,p", po::value<string>()->default_value("/tmp/ipna-ft-capture.pid"), "use this file to store the pid")
      ("verbose,v", "verbose output")
      ("ipv4,4", "use ipv4")
      ("ipv6,6", "use ipv6, that is the default but currently not implemented!")
      ("only-from,o", po::value<string>(), "fan out packets only from this source ip")
      ("listen,l", po::value<string>(), "listen on ip/port")
      ;
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
    
  // parse listen pair and check it
  IpPortPair listenPair = splitIntoIpPortPair( vm["listen"].as<string>() );
  if (!checkIpPortPair(listenPair)) {
    LOG_ERROR("invalid ip/port pair: " << vm["listen"].as<string>().c_str());
    exit(3);
  }

  LOG_DEBUG("listening on ip: " << listenPair.first << " port: " << listenPair.second);

  typedef boost::shared_ptr<ipna::network::Socket> SocketPtr;

  SocketPtr listenSocket;
  int error;
    
  try {
    listenSocket = SocketPtr(network::Socket::UDPSocket());
  } catch (string & s) {
    LOG_ERROR(s);
    exit(3);
  }
  if(listenSocket->bind(listenPair.first, listenPair.second) < 0) {
    perror("bind");
    LOG_ERROR("could not bind to listen-socket!");
    exit(3);
  }

  boost::shared_ptr<network::Listener> listener(new network::Listener(listenSocket));
  boost::shared_ptr<capture::CapturePacketHandler> handler(new capture::CapturePacketHandler);
  listener->addHandler(handler);

  listener->start();
}
