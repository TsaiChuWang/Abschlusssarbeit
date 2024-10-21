#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <unistd.h>

#include "../Enthalten/include.h"

// sudo ip netns exec ns2 gcc simple_V7_receive.c -o simple_V7_receive
// sudo ip netns exec ns2 ./simple_V7_receive

int main(int argc, char *argv[]) {
    int receive_socket;
    char buffer[MAX_BUFFER_SIZE];

    receive_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (receive_socket < 0) {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }

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

        fflush(stdout);
        printf("Received packet: Source IP: "INFORM_IP_ADDRESS_FORMAT", Source Port: "INFORM_PORT_FORMAT", Packet Size : "INFORM_PACKET_SIZE_FORMAT"\n", inet_ntoa(*(struct in_addr *)&iph->saddr), ntohs(udph->source), packet_length);
    }

    close(receive_socket);
    return EXIT_SUCCESS;
}
