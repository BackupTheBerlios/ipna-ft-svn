#ifndef SOCKET_H
#define SOCKET_H

#include <sys/types.h>
#include <netinet/in.h>
#include <string>

class Socket {
 private:
        int _fd;
        struct sockaddr_in _addr;
 protected:
        Socket();
 public:
        static Socket* newSocket(int domain, int type, int protocol);
        static Socket* UDPSocket();
        static Socket* TCPSocket();
        virtual ~Socket();

        ssize_t bind(std::string hostorip = "*", int port = 0);
        
        ssize_t recv(void* buffer, size_t len, int flags = 0);
        ssize_t send(const void* buffer, size_t len, int flags = 0);

        /* udp interface */
        ssize_t recvfrom(void* buffer, size_t len, struct sockaddr *from, int flags = 0);
        ssize_t sendto(const void* buffer, size_t len, const struct sockaddr *to, int flags = 0);
};

#endif // SOCKET_H
