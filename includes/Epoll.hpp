#ifndef Epoll_HPP
#define Epoll_HPP

#include "Global.hpp"
#include <vector>

class Epoll {
public:
    Epoll();
    ~Epoll();

    void addFd(int fd, uint32_t events);
    void modifyFd(int fd, uint32_t events);
    void removeFd(int fd);
    std::vector<struct epoll_event> waitForEvents();

private:
    int epollFd;
    struct epoll_event* events;
    int maxEvents;
};

#endif
