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

#include "Socket.h"
#include "Listener.h"
#include "PacketHandler.h"
#include "FanoutPacketHandler.h"
#include "Logger.hpp"

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
    
  // parse listen pair and check it
  IpPortPair listenPair = splitIntoIpPortPair( vm["listen"].as<string>() );
  if (!checkIpPortPair(listenPair)) {
    LOG_ERROR("invalid ip/port pair: " << vm["listen"].as<string>().c_str());
    exit(3);
  }

  // parse export pairs and check them
  vector<IpPortPair> exportPairs;
  vector<string> exports = vm["export"].as< vector<string> >();
  for (vector<string>::iterator it = exports.begin(); it != exports.end(); it++) {
    IpPortPair p = splitIntoIpPortPair(*it);
    if (!checkIpPortPair(p)) {
      LOG_ERROR("invalid ip/port pair: " << it->c_str());
      exit(3);
    }
    exportPairs.push_back(p);
  }

  LOG_DEBUG("listening on ip: " << listenPair.first << " port: " << listenPair.second);

  boost::shared_ptr<Socket> listenSocket;
  boost::shared_ptr<Socket> sendSocket;
  int error;
    
  try {
    listenSocket = boost::shared_ptr<Socket>(Socket::UDPSocket());
  } catch (string & s) {
    LOG_ERROR(s);
    exit(3);
  }
  if(listenSocket->bind(listenPair.first, listenPair.second) < 0) {
    perror("bind");
    LOG_ERROR("could not bind to listen-socket!");
    exit(3);
  }

  try {
    sendSocket = boost::shared_ptr<Socket>(Socket::UDPSocket());
  } catch (string & s) {
    LOG_ERROR(s);
    exit(3);
  }
  if (sendSocket->bind("*",0) < 0) {
    perror("bind");
    LOG_ERROR("could not bind to send-socket!");
    exit(3);
  }

  boost::shared_ptr<FanoutPacketHandler> fanout(new FanoutPacketHandler(sendSocket));
  boost::shared_ptr<Listener> listener(new Listener(listenSocket));
  listener->addHandler(fanout);
    
  // create destination infos:
  for (vector<IpPortPair>::iterator it = exportPairs.begin(); it != exportPairs.end(); it++) {
    FanoutPacketHandler::DestinationPtr to(new struct sockaddr_in);
    to->sin_family = AF_INET;
    to->sin_port = htons(it->second);
    to->sin_addr.s_addr = inet_addr(it->first.c_str());
    memset(&(to->sin_zero), '\0', 8);
    fanout->addDestination(to);
  }

  listener->start();
}
