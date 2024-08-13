#include "SignalHandler.hpp"

SignalHandler::SignalHandler() {}

SignalHandler::~SignalHandler() {}

SignalHandler& SignalHandler::getInstance() {
    static SignalHandler instance;
    return instance;
}

void SignalHandler::setupSignalHandlers(int *running, int efd) {
    this->running = running;
    this->efd = efd;
    signal(SIGINT, SignalHandler::signalHandler);
}

void SignalHandler::signalHandler(int signum) {
    if (signum == SIGINT) {
        uint64_t u = 1;
        std::cout << " Caught SIGINT, exiting..." << std::endl;
        SignalHandler& handler = SignalHandler::getInstance();
        write(handler.efd, &u, sizeof(uint64_t));
        *handler.running = 0;
    }
}
