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

#include "Socket.h"
#include "cnfp.h"

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
    int verbosity;

    // 1. parse options
    // 2. listen on specified source
    // 3. loop:
    //    send packets to specified destinations
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "show this help message")
        ("verbose,v", "verbose output, debuglevel = number of -v")
        ("ipv4,4", "use ipv4")
        ("ipv6,6", "use ipv6, that is the default but currently not implemented!")
        ("only-from,o", po::value<string>(), "fan out packets only from this source ip")
        ("listen,l", po::value<string>(), "listen on ip/port")
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

    verbosity = vm.count("verbose");

    // parse listen pair and check it
    IpPortPair listenPair = splitIntoIpPortPair( vm["listen"].as<string>() );
    if (!checkIpPortPair(listenPair)) {
        cerr << "ERROR: invalid ip/port pair: " << vm["listen"].as<string>() << endl;
        exit(3);
    }

    // parse export pairs and check them
    vector<IpPortPair> exportPairs;
    vector<string> exports = vm["export"].as< vector<string> >();
    for (vector<string>::iterator it = exports.begin(); it != exports.end(); it++) {
        IpPortPair p = splitIntoIpPortPair(*it);
        if (!checkIpPortPair(p)) {
            cerr << "ERROR: invalid ip/port pair: " << *it << endl;
            exit(3);
        }
        exportPairs.push_back(p);
    }

    if (verbosity > 1) {
        cout << "listening on ip:" << listenPair.first << " port:" << listenPair.second << endl;
        cout << "exporting to:";
        for (vector<IpPortPair>::iterator it = exportPairs.begin(); it != exportPairs.end(); it++) {
            cout << " ip:" << it->first << " port:" << it->second;
        }
        cout << endl;
    }

    Socket* listenSocket;
    Socket* sendSocket;
    int error;
    
    try {
        listenSocket = Socket::UDPSocket();
    } catch (string s) {
        cerr << s << endl;
        exit(3);
    }
    if(listenSocket->bind(listenPair.first, listenPair.second) < 0) {
        perror("bind");
        exit(3);
    }

    try {
        sendSocket = Socket::UDPSocket();
    } catch (string s) {
        cerr << s << endl;
        exit(3);
    }
    if (sendSocket->bind("*",0) < 0) {
        perror("bind");
        exit(3);
    }
    
    // create destination infos:
    vector< struct sockaddr_in > destinationAddr;
    for (vector<IpPortPair>::iterator it = exportPairs.begin(); it != exportPairs.end(); it++) {
        struct sockaddr_in to;
        to.sin_family = AF_INET;
        to.sin_port = htons(it->second);
        to.sin_addr.s_addr = inet_addr(it->first.c_str());
        memset(&(to.sin_zero), '\0', 8);
        destinationAddr.push_back(to);
    }

    unsigned int lastSequenceIdx = 0;
    const unsigned int MAXBUFLEN = 2048;
    char buffer[MAXBUFLEN];

    const unsigned int SEQLEN = 32;
    unsigned int sequenceNumber[SEQLEN] = {0};
//    for (unsigned int i = 0; i < SEQLEN; ++i)
//        sequenceNumber[i] = 0;

    for(;;) {
        struct sockaddr_in from;
        struct cnfp_v9_hdr header;
        unsigned int fromlen = sizeof(struct sockaddr);
        unsigned int newSequence;
        
        memset(buffer, 0, MAXBUFLEN);
        int received = listenSocket->recvfrom(buffer, MAXBUFLEN, (struct sockaddr*)&from, &fromlen);
        if (received < 0) {
            perror("recvfrom");
            delete listenSocket;
            delete sendSocket;
            exit(1);
        }

        // analyze a little bit
        header = *(struct cnfp_v9_hdr*)buffer;
        newSequence = ntohl(header.seq);
        
        if (verbosity > 0) {
            fprintf(stdout, "version:%d count:%d uptime:%d tstamp:%d seq:%d source:%d\n",
                    ntohs(header.common.version),
                    ntohs(header.common.count),
                    ntohl(header.uptime),
                    ntohl(header.tstamp),
                    ntohl(header.seq),
                    ntohl(header.engine_id)
                );
        }

        unsigned int foundIdx = SEQLEN;
        // look if we had the sequence number - 1 in our window... worst-case: O(n) best-case: O(1)
        for (unsigned int i = lastSequenceIdx; i != (lastSequenceIdx-1)%SEQLEN; i = (i+1)%SEQLEN) {
            if ((sequenceNumber[i]+1) == newSequence) {
                foundIdx = i;
                break;
            }
        }

        if (foundIdx != SEQLEN && ((sequenceNumber[lastSequenceIdx]+1) != newSequence)) {
            cerr << "WARN: packet-reordering occured within window: [" << sequenceNumber[foundIdx] << ":" << sequenceNumber[lastSequenceIdx] << "]" << endl;
        } else if (foundIdx == SEQLEN) {
            cerr << "WARN: missed " << (int)(newSequence - sequenceNumber[lastSequenceIdx]) << " packet(s)" << endl;
        }

        // store the new sequence
        lastSequenceIdx = (lastSequenceIdx+1)%SEQLEN;
        sequenceNumber[lastSequenceIdx] = newSequence;

        for (vector<struct sockaddr_in>::iterator it = destinationAddr.begin(); it != destinationAddr.end(); it++) {
            int sent = sendSocket->sendto(buffer, received, (struct sockaddr*)&(*it));
            if (sent == -1) {
                perror("sendto");
                delete listenSocket;
                delete sendSocket;
                exit(1);
            } else if (sent != received) {
                cerr << "WARN: could not send all data at once!" << endl;
            }
        }
    }
}
