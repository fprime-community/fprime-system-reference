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

volatile sig_atomic_t terminate = 0;

static void sighandler(int signum) {
    SystemReference::teardown(state);
    terminate = 1;
}

void run1cycle() {
    // call interrupt to emulate a clock
    SystemReference::blockDrv.callIsr();
    Os::Task::delay(Fw::TimeInterval(1, 0)); //1Hz
}

void runcycles(FwSizeType cycles) {
    if (cycles == -1) {
        while (true) {
            run1cycle();
        }
    }

    for (FwIndexType cycle = 0; cycle < cycles; cycle++) {
        run1cycle();
    }
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

    int cycle = 0;

    while (!terminate) {
//        (void) printf("Cycle %d\n",cycle);
        runcycles(1);
        cycle++;
    }

    // Give time for threads to exit
    (void) printf("Waiting for threads...\n");
    Os::Task::delay(Fw::TimeInterval(1, 0));

    (void) printf("Exiting...\n");

    return 0;
}
