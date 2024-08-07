#include "SignalHandler.hpp"

SignalHandler::SignalHandler() {}

SignalHandler::~SignalHandler() {}

SignalHandler& SignalHandler::getInstance() {
    static SignalHandler instance;
    return instance;
}

void SignalHandler::setupSignalHandlers(const std::vector<int>& sockets) {
    this->sockets = sockets;
    signal(SIGINT, SignalHandler::signalHandler);
}

void SignalHandler::signalHandler(int signum) {
    if (signum == SIGINT) {
        std::cout << " Caught SIGINT, exiting..." << std::endl;
        SignalHandler& handler = SignalHandler::getInstance();
        for (std::vector<int>::iterator it = handler.sockets.begin(); it != handler.sockets.end(); ++it) {
            close(*it);
        }
        exit(EXIT_FAILURE);
    }
}

