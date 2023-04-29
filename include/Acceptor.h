class Reactor;

class Acceptor
{
private:
    Reactor *reactor_;
    int listen_fd_;

public:
    Acceptor(Reactor *reactor, int listen_fd);
    void handleEvent(uint32_t events);
};