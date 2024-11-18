#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <unistd.h>

#include "../Enthalten/netnamespce.h"

#define DEST_IP "10.0.0.2"  // Target IP in ns2

// sudo ip netns exec ns1 gcc simple_V6_send.c -o simple_V6_send
// sudo ip netns exec ns1 ./simple_V6_send

// sudo ip netns delete ns1
// sudo ip netns delete ns2


int main() {
    // int sock;
    // struct sockaddr_in dest_addr;
    // char packet[4096];

    // // Create raw socket
    // sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    // if (sock < 0) {
    //     perror("Socket error");
    //     exit(1);
    // }

    // // IP Header
    // struct iphdr *iph = (struct iphdr *) packet;
    // struct udphdr *udph = (struct udphdr *) (packet + sizeof(struct iphdr));

    // memset(packet, 0, 4096);  // Clear packet buffer

    // // Fill in the IP Header
    // iph->ihl = 5;
    // iph->version = 4;
    // iph->tos = 0;
    // iph->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr);
    // iph->id = htonl(54321);
    // iph->frag_off = 0;
    // iph->ttl = 64;
    // iph->protocol = IPPROTO_UDP;
    // iph->saddr = inet_addr("10.0.0.1");
    // iph->daddr = inet_addr(DEST_IP);
    // iph->check = checksum((unsigned short *)iph, iph->tot_len);

    // // UDP Header
    // udph->source = htons(12345);
    // udph->dest = htons(54321);
    // udph->len = htons(sizeof(struct udphdr));
    // udph->check = 0;  // No checksum for simplicity

    // // Destination address
    // dest_addr.sin_family = AF_INET;
    // dest_addr.sin_addr.s_addr = inet_addr(DEST_IP);

    // // Send the packet
    // if (sendto(sock, packet, iph->tot_len, 0, (struct sockaddr *) &dest_addr, sizeof(dest_addr)) < 0) {
    //     perror("Send error");
    //     exit(1);
    // }
    
    // printf("Packet sent from ns1 to %s\n", DEST_IP);
    // close(sock);
    return 0;
}
