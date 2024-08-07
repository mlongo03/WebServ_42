#include "Epoll.hpp"

Epoll::Epoll() {
    epollFd = epoll_create1(0);
    if (epollFd == -1) {
        throw std::runtime_error("epoll_create1 error");
    }

    maxEvents = 10;
    events = new struct epoll_event[maxEvents];
}

Epoll::~Epoll() {
    close(epollFd);
    delete[] events;
}

void Epoll::addFd(int fd, uint32_t events) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;

    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        throw std::runtime_error("epoll_ctl add error");
    }
}

void Epoll::modifyFd(int fd, uint32_t events) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;

    if (epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &ev) == -1) {
        throw std::runtime_error("epoll_ctl mod error");
    }
}

void Epoll::removeFd(int fd) {
    if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL) == -1) {
        throw std::runtime_error("epoll_ctl del error");
    }
}

std::vector<struct epoll_event> Epoll::waitForEvents() {
    int eventCount = epoll_wait(epollFd, events, maxEvents, -1);
    if (eventCount == -1) {
        throw std::runtime_error("epoll_wait error");
    }

    return std::vector<struct epoll_event>(events, events + eventCount);
}
