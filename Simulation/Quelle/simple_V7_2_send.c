#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <unistd.h>

#include "../Enthalten/include.h"
#define CHECK_SUM
#include "../Enthalten/netnamespce.h"
#include "../Enthalten/traffic_generation.h"

#define MEAN 20
#define STANDARD_DEVIATION 10

// sudo ip netns exec [ns.name] gcc simple_V7_2_send.c -o ../Ausführung/simple_V7_2_send
// sudo ip netns exec ns2 gcc simple_V7_2_send.c -o ../Ausführung/simple_V7_2_send
// sudo ip netns exec [ns.name] ../Ausführung/simple_V7_2_send [ns.ip] [destination IP] [ns.id] [ns.name] [time_interval]
// sudo ip netns exec ns2 ../Ausführung/simple_V7_2_send 10.0.0.2 10.0.0.1 2 ns2 10

int main(int argc, char *argv[]) {

    // printf("Packet sent from ns1 to %s\n", argv[2]);
    int simulation_time = atoi(argv[5]);

    char* ns_name = argv[4];
    char* source_ip_address = argv[1];
    char* destination_ip_address = argv[2];
    int identifier = atoi(argv[3]);

    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "sudo ip netns exec "INFORM_NS_NAME_FORMAT" gcc ./simple_V7_1_send.c -o ../Ausführung/simple_V7_1_send", ns_name);
    system(command);  
    // printf("%s\n", command);
    // sprintf(command, "sudo ip netns exec ns2 ../Ausführung/simple_V7_1_send 10.0.0.2 10.0.0.1 2 5");
    // system("sudo ip netns exec ns2 ../Ausführung/simple_V7_1_send 10.0.0.2 10.0.0.1 2 5"); 
    // printf("%s\n", command);
    int* traffic = generateNormalDistribution(simulation_time, MEAN, STANDARD_DEVIATION);
    setbuf(stdout, NULL);
    for(int time_stamp = 0;time_stamp<simulation_time;time_stamp++){
        fflush(stdout);
        sprintf(command, "sudo ip netns exec %s ../Ausführung/simple_V7_1_send %s %s %d %d", ns_name, source_ip_address, destination_ip_address, identifier, *(traffic+time_stamp));;
        // // sprintf(command, "sudo ip netns exec "INFORM_NS_NAME_FORMAT" ../Ausführung/simple_V7_1_send "INFORM_IP_ADDRESS_FORMAT" %s %d %d", ns_name, source_ip_address, identifier, destination_ip_address, *(traffic+time_stamp));
        system(command); 
        // printf("%s\n", command);
    }
    return EXIT_SUCCESS;
}
