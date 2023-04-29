#include "TcpServer.h"
#include "Reactor.h"

TcpServer::TcpServer()
{
}

void TcpServer::start(int port)
{
    fd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (fd_ == -1)
    {
        perror("socket");
        return false;
    }

    int flags = fcntl(fd_, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl F_GETFL");
        return false;
    }

    if (fcntl(fd_, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("fcntl F_SETFL O_NONBLOCK");
        return false;
    }

    int reuseaddr = 1;
    if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) == -1)
    {
        perror("setsockopt SO_REUSEADDR");
        return false;
    }

    int reuseport = 1;
    if (setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &reuseport, sizeof(int)) == -1)
    {
        perror("setsockopt SO_REUSEPORT");
        return false;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(fd_, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind");
        return false;
    }

    if (listen(fd_, 10) == -1)
    {
        perror("listen");
        return false;
    }

    reactor_ = new Reactor(fd_);
    reactor_.run();
}