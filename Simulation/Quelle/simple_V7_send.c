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

// sudo ip netns exec [ns.name] gcc simple_V7_send.c -o ../Ausführung/simple_V7_send
// sudo ip netns exec [ns.name] ../Ausführung/simple_V7_send [ns.ip] [destination IP] [ns.id]
// sudo ip netns exec ns2 ../Ausführung/simple_V7_send 10.0.0.2 10.0.0.1 2

int main(int argc, char *argv[]) {
    int send_socket;
    struct sockaddr_in dest_addr;
    char packet[MAX_BUFFER_SIZE];

    send_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (send_socket < 0) {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }

    struct iphdr *ipheader = (struct iphdr *) packet;
    struct udphdr *udpheader = (struct udphdr *) (packet + sizeof(struct iphdr));

    memset(packet, 0, MAX_BUFFER_SIZE);  // Clear packet buffer

    // Fill in the IP Header
    ipheader->ihl = IP_HEADER_LENGTH;
    ipheader->version = IP_VERSION;
    ipheader->tos = IP_TYPE_OF_SERVICE;
    ipheader->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr);
    ipheader->id = htonl(atoi(argv[3]));
    ipheader->frag_off = 0;
    ipheader->ttl = TIME_TO_LIVE;
    ipheader->protocol = IPPROTO_UDP;
    ipheader->saddr = inet_addr(argv[1]);
    ipheader->daddr = inet_addr(argv[2]);
    ipheader->check = checksum((unsigned short *)ipheader, ipheader->tot_len);

    // UDP Header
    udpheader->source = htons(UDP_SEND_PORT);
    udpheader->dest = htons(UDP_RECEIVE_PORT);
    udpheader->len = htons(sizeof(struct udphdr));
    udpheader->check = 0;  // No checksum for simplicity

    // Destination address
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(argv[2]);

    // Send the packet
    if (sendto(send_socket, packet, ipheader->tot_len, 0, (struct sockaddr *) &dest_addr, sizeof(dest_addr)) < 0) {
        perror("Send error");
        exit(EXIT_FAILURE);
    }
    
    // printf("Packet sent from ns1 to %s\n", argv[2]);
    close(send_socket);
    return EXIT_SUCCESS;
}
