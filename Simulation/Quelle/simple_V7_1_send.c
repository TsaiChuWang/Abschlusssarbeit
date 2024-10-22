#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <unistd.h>
#include <time.h>

#include "../Enthalten/include.h"
#define CHECK_SUM
#define _NET_NAMESPCE
#include "../Enthalten/netnamespce.h"

// sudo ip netns exec [ns.name] gcc simple_V7_1_send.c -o ../Ausführung/simple_V7_1_send
// sudo ip netns exec [ns.name] ../Ausführung/simple_V7_1_send [ns.ip] [ns.id] [packet number persecond]

// sudo ip netns exec ns2 gcc simple_V7_1_send.c -o ../Ausführung/simple_V7_1_send
// sudo ip netns exec ns2 ../Ausführung/simple_V7_1_send 10.0.0.2 2 5
// sudo ip netns exec ns2 ../Ausführung/simple_V7_1_send 10.0.0.2 10.0.0.1 2 5

int main(int argc, char *argv[]) {
    int packet_count = atoi(argv[4]);
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    // printf("Current time: %ld.%09ld seconds\n", ts.tv_sec, ts.tv_nsec);
    long second = ts.tv_sec;
    long nsecond =  ts.tv_nsec;
    // 0.000048312

    for (int i = 0; i < packet_count; i++) {
        fflush(stdout);
        char command[MAX_COMMAND_LENGTH];
        sprintf(command, "sudo ip netns exec ns2 ../Ausführung/simple_V7_send "INFORM_IP_ADDRESS_FORMAT" "INFORM_IP_ADDRESS_FORMAT" "INFORM_IDENTIFIER_FORMAT, argv[1], argv[2], atoi(argv[3]));
        system(command);

        clock_gettime(CLOCK_REALTIME, &ts);
        // printf("Current time: %ld.%09ld seconds\n", ts.tv_sec-second, ts.tv_nsec-nsecond);
        // usleep(1000000 / packet_count);  // Sleep to control packet rate
        // printf("%ld\n",(750000 - ((packet_count/10)*100000)) );
        usleep((687500 - ((packet_count/10)*100000)) / packet_count);
    }

// clock_gettime(CLOCK_REALTIME, &ts);
//         printf("Current time: %ld.%09ld seconds\n", ts.tv_sec-second, ts.tv_nsec-nsecond);

    return EXIT_SUCCESS;
}
