#include "Socket.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

Socket::Socket() {
    _fd = -1;
}

Socket* Socket::newSocket(int domain, int type, int protocol) {
    Socket* s = new Socket;
    s->_fd = socket(domain,type,protocol);
    if (s->_fd < 0) {
        perror("socket");
        throw "could not open socket!";
    }
    return s;
}

Socket* Socket::UDPSocket() {
    return newSocket(PF_INET, SOCK_DGRAM, 0);
}

Socket* Socket::TCPSocket() {
    return newSocket(PF_INET, SOCK_STREAM, 0);
}

Socket::~Socket() {
    if (_fd >= 0) {
        close(_fd);
    }
}

ssize_t Socket::recv(void* buffer, size_t len, int flags) {
    extern ssize_t recv(int,void*,size_t,int);
    return recv(_fd, buffer, len, flags);
}

ssize_t Socket::send(const void* buffer, size_t len, int flags) {
    extern ssize_t send(int,const void*,size_t,int);
    return send(_fd, buffer, len, flags);
}

ssize_t Socket::recvfrom(void* buffer, size_t len, struct sockaddr *from, int flags) {
    extern ssize_t recvfrom(int,void*,size_t,int,struct sockaddr*,size_t);
    return recvfrom(_fd, buffer, len, flags, from, sizeof(struct sockaddr));
}

ssize_t Socket::sendto(const void* buffer, size_t len, const struct sockaddr *to, int flags) {
    extern ssize_t sendto(int,const void*,size_t,int,const struct sockaddr*,size_t);
    return sendto(_fd, buffer, len, flags, to, sizeof(struct sockaddr));
}

int Socket::bind(std::string hostorip, int port) {
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
    if (hostorip == "*") {
        _addr.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        _addr.sin_addr.s_addr = inet_addr(hostorip.c_str());
    }
    memset(&(_addr.sin_zero), '\0', 8);

    extern int bind(int,const struct sockaddr*, socklen_t);
    return bind(_fd,(struct sockaddr*)&_addr,sizeof(struct sockaddr));
}
