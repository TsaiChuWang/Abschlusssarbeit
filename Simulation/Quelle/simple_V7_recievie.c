#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <unistd.h>

// sudo ip netns exec ns2 gcc simple_V6_receive.c -o simple_V6_receive
// sudo ip netns exec ns2 ./simple_V6_receive

int main() {
    int receive_sock;
    char buffer[4096];

    // Create raw socket for receiving
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sock < 0) {
        perror("Socket error");
        exit(1);
    }

    printf("Waiting for packet...\n");

    // Receive packet
    while (1) {
        int packet_len = recvfrom(sock, buffer, 4096, 0, NULL, NULL);
        if (packet_len < 0) {
            perror("Receive error");
            exit(1);
        }

        struct iphdr *iph = (struct iphdr *) buffer;
        struct udphdr *udph = (struct udphdr *) (buffer + sizeof(struct iphdr));

        printf("Received packet: Source IP: %s, Source Port: %d\n",
               inet_ntoa(*(struct in_addr *)&iph->saddr), ntohs(udph->source));
    }

    close(sock);
    return 0;
}
