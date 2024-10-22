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

// sudo ip netns exec ns2 gcc simple_V7_receive.c -o simple_V7_receive
// sudo ip netns exec ns2 ./simple_V7_receive

int main(int argc, char *argv[]) {
    int receive_socket;
    char buffer[MAX_BUFFER_SIZE];

    int packet_count = 0;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    printf("Current time: %ld.%09ld seconds\n", ts.tv_sec, ts.tv_nsec);
    long second = ts.tv_sec;
    long nsecond =  ts.tv_nsec;
    int record = 0;

    receive_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (receive_socket < 0) {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }

    setbuf(stdout, NULL);
    fflush(stdout);
    printf("Waiting for packet...\n");

    while (TRUE) {
        int packet_length = recvfrom(receive_socket, buffer, MAX_BUFFER_SIZE, 0, NULL, NULL);
        if (packet_length < 0) {
            perror("Receive error");
            exit(EXIT_FAILURE);
        }

        struct iphdr *iph = (struct iphdr *) buffer;
        struct udphdr *udph = (struct udphdr *) (buffer + sizeof(struct iphdr));
        packet_count +=1;



        setbuf(stdout, NULL);
        fflush(stdout);
        clock_gettime(CLOCK_REALTIME, &ts);
        if(record==0){
            second = ts.tv_sec;
            nsecond =  ts.tv_nsec;
            record = 1;
        }
        // printf("a\n");
        printf("time : %ld.%09ld Received packet: Source IP: "INFORM_IP_ADDRESS_FORMAT", Source Port: "INFORM_PORT_FORMAT", Packet Size : "INFORM_PACKET_SIZE_FORMAT" Packet Count : %d\n", ts.tv_sec- second, ts.tv_nsec - nsecond, inet_ntoa(*(struct in_addr *)&iph->saddr), ntohs(udph->source), packet_length, packet_count);

    }

    close(receive_socket);
        
    return EXIT_SUCCESS;
}
