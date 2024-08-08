#ifndef SIGNALHANDLER_HPP
#define SIGNALHANDLER_HPP

#include "Global.hpp"
#include "Worker.hpp"

class SignalHandler {
public:
    static SignalHandler& getInstance();

    void setupSignalHandlers(const std::vector<int>& sockets, Worker *worker);

private:
    SignalHandler();  // Private constructor
    ~SignalHandler();
    SignalHandler(const SignalHandler&);  // Copy constructor
    SignalHandler& operator=(const SignalHandler&);  // Assignment operator
    static void signalHandler(int signum);

    std::vector<int> sockets;
    Worker *worker;
};

#endif


