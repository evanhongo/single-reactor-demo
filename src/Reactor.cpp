#include "Reactor.h"

Reactor::Reactor(int listen_fd) : listen_fd_(listen_fd), epoll_fd_(::epoll_create1(EPOLL_CLOEXEC)), acceptor_(make_unique<Acceptor>(this, listen_fd))
{
}

Reactor::registerHandler(Handler *handler)
{
    int client_fd = handler->getClientFd();
    struct epoll_event event;
    event.data.fd = client_fd;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &event) == -1)
    {
        perror("epoll_ctl EPOLL_CTL_ADD conn_fd");
        close(client_fd);
        return;
    }

    handlers_[client_fd] = handler;
}

Reactor::removeHandler(Handler *handler)
{
    int client_fd = handler->getClientFd();
    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, client_fd, nullptr);
    handlers.erase(client_fd);
    delete handler;
    close(client_fd);
}

Reactor::dispatch(epoll_event event)
{
    if (event.data.fd == listen_fd_)
        acceptor_->handleEvent(event.events);
    else
    {
        Handler *handler = handlers_[event.data.fd];
        handler->handleEvent(event.events);
    }
}

Reactor::run()
{
    epoll_event events[max_num_events];
    while (true)
    {
        int num_events = epoll_wait(epoll_fd, events, max_num_events, -1);
        for (int i = 0; i < num_events; i++)
            dispatch(events[i])
    }
}