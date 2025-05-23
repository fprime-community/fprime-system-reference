#include <getopt.h>
#include <cstdlib>
#include <ctype.h>

#include <Os/Console.hpp>
#include <SystemReference/Top/SystemReferenceTopologyAc.hpp>

void print_usage(const char* app) {
    (void) printf("Usage: ./%s [options]\n-p\tport_number\n-a\thostname/IP address\n",app);
}

#include <signal.h>
#include <cstdio>

SystemReference::TopologyState state;
// Enable the console logging provided by Os::Log
Os::Console logger;

void stopSimulatedCycle() {
    linuxTimer.quit();
}

static void signalHandler(int signum) {
    stopSimulatedCycle();
}

void startSimulatedCycle(Fw::TimeInterval interval) {
    linuxTimer.startTimer(interval.getSeconds()*1000+interval.getUSeconds()/1000);
}

int main(int argc, char* argv[]) {
    U32 port_number = 0; // Invalid port number forced
    I32 option;
    char *hostname;
    option = 0;
    hostname = nullptr;

    while ((option = getopt(argc, argv, "hp:a:")) != -1){
        switch(option) {
            case 'h':
                print_usage(argv[0]);
                return 0;
                break;
            case 'p':
                port_number = static_cast<U32>(atoi(optarg));
                break;
            case 'a':
                hostname = optarg;
                break;
            case '?':
                return 1;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    (void) printf("Hit Ctrl-C to quit\n");

    state = SystemReference::TopologyState(hostname, port_number);
    SystemReference::setup(state);

    // register signal handlers to exit program
    signal(SIGINT,sighandler);
    signal(SIGTERM,sighandler);

    SystemReference::startSimulatedCycle(Fw::TimeInterval(1, 0));  // Program loop cycling rate groups at 1Hz
    SystemReference::teardownTopology(inputs);

    (void) printf("Exiting...\n");

    return 0;
}
