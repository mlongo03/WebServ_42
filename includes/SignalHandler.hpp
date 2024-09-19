#ifndef SIGNALHANDLER_HPP
#define SIGNALHANDLER_HPP

#include "Global.hpp"
#include "Worker.hpp"
#include "Client.hpp"

class SignalHandler {
public:
    static SignalHandler& getInstance();
    void setupSignalHandlers(int *running, int efd);
    static void signalHandler(int signum);

private:
    SignalHandler();
    ~SignalHandler();
    SignalHandler(const SignalHandler&);
    SignalHandler& operator=(const SignalHandler&);

    int *running;
    int efd;
};

#endif


