#include "Socket.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

static ssize_t RECV(int fd,void* b,size_t len,int f) {
    return recv(fd,b,len,f);
}

static ssize_t SEND(int fd,const void* b,size_t len,int f) {
    return send(fd,b,len,f);
}

static ssize_t SENDTO(int fd,const void* b,size_t len, int flags, const struct sockaddr* to, size_t tolen) {
    return sendto(fd, b, len, flags, to, tolen);
}

static ssize_t RECVFROM(int fd,void* b,size_t len,int flags,struct sockaddr* from, size_t* fromlen) {
    return recvfrom(fd,b,len,flags,from,fromlen);
}

static int BIND(int fd, const struct sockaddr* addr, socklen_t len) {
    return bind(fd,addr,len);
}

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
    return RECV(_fd, buffer, len, flags);
}

ssize_t Socket::send(const void* buffer, size_t len, int flags) {
    return SEND(_fd, buffer, len, flags);
}

ssize_t Socket::recvfrom(void* buffer, size_t len, struct sockaddr *from, size_t* fromlen, int flags) {
    return RECVFROM(_fd, buffer, len, flags, from, fromlen);
}

ssize_t Socket::sendto(const void* buffer, size_t len, const struct sockaddr *to, int flags) {
    return SENDTO(_fd, buffer, len, flags, to, sizeof(struct sockaddr));
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

    return BIND(_fd,(struct sockaddr*)&_addr,sizeof(struct sockaddr));
}
