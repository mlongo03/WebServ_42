#ifndef SIGNALHANDLER_HPP
#define SIGNALHANDLER_HPP

#include "Global.hpp"
#include "Worker.hpp"
#include "Client.hpp"

class SignalHandler {
public:
    static SignalHandler& getInstance();
    void setupSignalHandlers(int *running);
    static void signalHandler(int signum);

private:
    SignalHandler();  // Private constructor
    ~SignalHandler();
    SignalHandler(const SignalHandler&);  // Copy constructor
    SignalHandler& operator=(const SignalHandler&);  // Assignment operator

    int *running;
};

#endif


