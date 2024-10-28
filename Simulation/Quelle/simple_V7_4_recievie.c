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

// sudo ip netns exec ns1 gcc simple_V7_4_recievie.c -o ../Ausführung/simple_V7_4_recievie
// sudo ip netns exec ns1 ../Ausführung/simple_V7_4_recievie 2

#define TAU 8333333
#define L 16666666

typedef struct{
    long tau;    
    long l;   
    long last_time;
    int record;
    char* ip_address;
}GCRA;

GCRA initGCRA(char* ip_address){
    GCRA gcra;
    gcra.tau = TAU;
    gcra.l = L;
    gcra.last_time = 0;
    gcra.record = 0;
    gcra.ip_address = ip_address;
    return gcra;
}

int findBucket(GCRA* buckets, int tenant_number,char* ip_address, int* current_index){
    for(int index=0;index<tenant_number;index++)
        if(strcmp((char*)buckets[index].ip_address, ip_address)== 0)
            return index;
    
    // buckets[*current_index].ip_address = ip_address;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    buckets[*current_index].last_time = (long)ts.tv_sec* (long)ONE_SECOND_IN_NS + (long)ts.tv_nsec;
    buckets[*current_index].record = 1;
    buckets[*current_index].ip_address = (char*)malloc(sizeof(char)*(strlen(ip_address)+1));
    strcpy(buckets[*current_index].ip_address, ip_address);
    *current_index+=1;

    // printf("%d %s %ld\n", *current_index-1, buckets[*current_index].ip_address, buckets[*current_index].last_time);
    return *current_index-1;
}

int main(int argc, char *argv[]) {
    int tenant_number = atoi(argv[1]);
    int current_index = 0;
    GCRA* buckets = (GCRA*)malloc(sizeof(GCRA)*tenant_number);
    for(int index=0;index<tenant_number;index++)
        *(buckets+index) = initGCRA("0");
    
    int receive_socket;
    char buffer[MAX_BUFFER_SIZE];

    FILE* data = fopen("../Datei/simple_V7/data.csv", "w+");

    int packet_count = 0;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    // printf("Current time: %ld.%09ld seconds\n", ts.tv_sec, ts.tv_nsec);
    long second = ts.tv_sec;
    long nsecond =  ts.tv_nsec;
    int record = 0;

    struct sockaddr_in servaddr, cliaddr;

    receive_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (receive_socket < 0) {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("10.0.0.1");  // The IP of ns2
    servaddr.sin_port = htons(5001);

    if (bind(receive_socket, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(receive_socket);
        exit(EXIT_FAILURE);
    }
          
    setbuf(stdout, NULL);
    fflush(stdout);
    printf("Waiting for packet...\n");

    long virtual_time = 0;
    long allow_time = 0;

    while (TRUE) {
        int packet_length = recvfrom(receive_socket, buffer, MAX_BUFFER_SIZE, 0, NULL, NULL);
        socklen_t len = sizeof(cliaddr);
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
        // printf("d = %d\n", findBucket(buckets, tenant_number, inet_ntoa(*(struct in_addr *)&iph->saddr), &current_index));
        long elapsed_time_ns = (ts.tv_sec- second) * ONE_SECOND_IN_NS + (ts.tv_nsec - nsecond);
        printf("time : %ld.%09ld Received packet: Source IP: "INFORM_IP_ADDRESS_FORMAT", Source Port: "INFORM_PORT_FORMAT", Packet Size : "INFORM_PACKET_SIZE_FORMAT" Packet Count : %d\n", ts.tv_sec- second, ts.tv_nsec - nsecond, inet_ntoa(*(struct in_addr *)&iph->saddr), ntohs(udph->source), packet_length, packet_count);
        int bucket_index = findBucket(buckets, tenant_number, inet_ntoa(*(struct in_addr *)&iph->saddr), &current_index);
        long last_time_interval = elapsed_time_ns - buckets[bucket_index].last_time;
        buckets[bucket_index].last_time = elapsed_time_ns;

        int accept = 0;
        if(elapsed_time_ns>=allow_time){
            accept = 1;
            virtual_time = (virtual_time>elapsed_time_ns)?virtual_time+buckets[bucket_index].tau:elapsed_time_ns+buckets[bucket_index].tau;
            allow_time = virtual_time - buckets[bucket_index].l;
        }


        fprintf(data, "%ld, %s, %d, %d, %d, %d, %ld, %ld, %ld, %d\n", elapsed_time_ns, inet_ntoa(*(struct in_addr *)&iph->saddr), ntohs(udph->source), packet_length, packet_count, bucket_index, last_time_interval, virtual_time, allow_time, accept);
        
        // if (sendto(receive_socket, RESPONSE_MSG, strlen(RESPONSE_MSG), 0, (struct sockaddr *)&cliaddr, len) < 0) {
        //     perror("Send response failed");
        // } else {
        //     // printf("Sent ACK to %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
        // }
    }
    printf("coumt = %d\n", packet_count);
    fclose(data);
    close(receive_socket);
        
    return EXIT_SUCCESS;
}
