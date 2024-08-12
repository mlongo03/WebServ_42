#include "SignalHandler.hpp"

SignalHandler::SignalHandler() {}

SignalHandler::~SignalHandler() {}

SignalHandler& SignalHandler::getInstance() {
    static SignalHandler instance;
    return instance;
}

void SignalHandler::setupSignalHandlers(int *running) {
    this->running = running;
    signal(SIGINT, SignalHandler::signalHandler);
}

void SignalHandler::signalHandler(int signum) {
    if (signum == SIGINT) {
        std::cout << " Caught SIGINT, exiting..." << std::endl;
        SignalHandler& handler = SignalHandler::getInstance();
        *handler.running = 0;
    }
}
