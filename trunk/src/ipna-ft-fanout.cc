#include <iostream>
#include <sstream>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace po = boost::program_options;
using namespace std;

typedef int Port;
typedef string Ip;
typedef pair<Ip, Port> IpPortPair;

bool checkIpPortPair(IpPortPair& p) {
    Ip ip = p.first;
    Port port = p.second;

    // check port
    if (port < 0 || port > 65535) {
        return false;
    }

    struct hostent *hostCheck = gethostbyname(p.first.c_str());
    if (NULL == hostCheck) {
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
        cerr << "ERROR: invalid ip:port! " << s << endl;
        exit(1);
    }

    p.first = splitted[0];
    p.second = parseString<int>(splitted[1]);

    return p;
}

int main(int argc, char** argv) {
    string listenString;
    
    // 1. parse options
    // 2. listen on specified source
    // 3. loop:
    //    send packets to specified destinations
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "show this help message")
        ("4,ipv4", "use ipv4")
        ("6,ipv6", "use ipv6, that is the default but currently not implemented!")
        ("only-from,o", po::value<string>(), "fan out packets only from this source ip")
        ("listen,l", po::value<string>(&listenString), "listen on ip/port")
        ("export,e", po::value< vector<string> >(), "export to these probes (ip/port)")
        ;
    po::positional_options_description p;
    p.add("export", -1);
    po::variables_map vm;
    po::store(po::command_line_parser(argc,argv).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cerr << desc << endl;
        exit(0);
    }

    if (0 == vm.count("listen")) {
        cerr << "ERROR: no ip/port pair specified to listen on!" << endl;
        cerr << desc << endl;
        exit(1);
    }

    if (0 == vm.count("export")) {
        cerr << "ERROR: no ip/port pair specified to export to!" << endl;
        cerr << desc << endl;
        exit(2);
    }

    // parse listen pair and check it
    IpPortPair listenPair = splitIntoIpPortPair( vm["listen"].as<string>() );
    if (!checkIpPortPair(listenPair)) {
        cerr << "ERROR: invalid ip/port pair: " << vm["listen"].as<string>() << endl;
        exit(3);
    }

    // parse export pairs and check them
    vector<IpPortPair> exportPairs;
    vector<string> exports = vm["export"].as< vector<string> >();
    for (unsigned int i = 0; i < exports.size(); ++i) {
        IpPortPair p = splitIntoIpPortPair(exports[i]);
        if (!checkIpPortPair(p)) {
            cerr << "ERROR: invalid ip/port pair: " << exports[i] << endl;
            exit(3);
        }
        exportPairs.push_back(p);
    }

    cout << "listening on ip:" << listenPair.first << " port:" << listenPair.second << endl;
    cout << "exporting to:";
    for (unsigned int i = 0; i < exports.size(); ++i) {
        IpPortPair p = exportPairs[i];
        cout << " ip:" << p.first << " port:" << p.second;
    }
    cout << endl;

    int listenFd;
    int sendFd;

    struct sockaddr_in my_addr;
    struct sockaddr_in my_send_addr;
    
    // create listen socket
    listenFd = socket(PF_INET, SOCK_DGRAM, 0);
    if (listenFd < 0) {
        perror("socket");
        exit(3);
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(listenPair.second);
    my_addr.sin_addr.s_addr = inet_addr(listenPair.first.c_str());
    memset(&(my_addr.sin_zero), '\0', 8);
    
    int yes = 1;
    if (setsockopt(listenFd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    // create send socket
    sendFd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sendFd < 0) {
        perror("socket");
        exit(3);
    }

    my_send_addr.sin_family = AF_INET;
    my_send_addr.sin_port = htons(0);
    my_send_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&(my_send_addr.sin_zero), '\0', 8);
    
    if (setsockopt(sendFd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    // listen
    if (bind(listenFd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) == -1 ) {
        perror("bind");
        exit(1);
    }

    // create destination infos:
    vector< struct sockaddr_in > destinationAddr;
    for (unsigned int i = 0; i < exports.size(); ++i) {
        IpPortPair p = exportPairs[i];
        struct sockaddr_in to;
        to.sin_family = AF_INET;
        to.sin_port = htons(p.second);
        to.sin_addr.s_addr = inet_addr(p.first.c_str());
        memset(&(to.sin_zero), '\0', 8);
        destinationAddr.push_back(to);
    }
    
    const unsigned int MAXBUFLEN = 2048;
    char buffer[MAXBUFLEN];
    
    for(;;) {
        struct sockaddr_in from;
        unsigned int from_len = sizeof(struct sockaddr);
        memset(buffer, 0, MAXBUFLEN);
        int received = recvfrom(listenFd, &buffer[0], MAXBUFLEN, 0 /*flags*/, (struct sockaddr*)&from, &from_len);
        if (received < 0) {
            perror("recvfrom");
            close(listenFd);
            close(sendFd);
            exit(1);
        }
//        cout << "received data of size: " << received << endl;
//        cout << "+";
        // forward it...
        for (unsigned int i = 0; i < destinationAddr.size(); ++i) {
            int sent = sendto(sendFd, &buffer[0], received, 0, (struct sockaddr*)&(destinationAddr[i]), sizeof(struct sockaddr));
            if (sent == -1) {
                perror("sendto");
                close(listenFd);
                close(sendFd);
                exit(1);
            } else if (sent != received) {
                cerr << "WARN: could not send all data at once!" << endl;
            }
//            cout << "-";
//            cout.flush();
        }
    }
}
