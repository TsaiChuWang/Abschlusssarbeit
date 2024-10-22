#define IP_HEADER_LENGTH 5
#define IP_VERSION 4
#define IP_TYPE_OF_SERVICE 0
#define TIME_TO_LIVE 64
#define UDP_RECEIVE_PORT 12345
#define UDP_SEND_PORT 54321

#define MAX_VETH_A_BRIDGE 100

#ifdef _NET_NAMESPCE

#include <string.h>
#include "../Enthalten/include.h"
#include <time.h>
#include <pthread.h>
#include <unistd.h>  // For nanosleep
#include "../Enthalten/traffic_generation.h"

#define STATUS_ANNOUCED       0
#define STATUS_CREATED        1
#define STATUS_IP_CONFIGURED  2
#define STATUS_DELETED        3

#define RECEIVE_PROGRAM "simple_V7_recievie"
#define SEND_PROGRAM "simple_V7_send"
#define SEND_PROGRAM_SPECIEFIED_PACKET_NUMBER_PER_SECOND "simple_V7_1_send"
#define SEND_PROGRAM_TRAFFIC_GENERATION "simple_V7_2_send"

#define DESTINATION_IP "10.0.0.1"
#define PACKET_SIZE 256

typedef struct{
  unsigned int identifier;
  char* name;
  char* ip_address;

  int status;
}NetworkNamesapce;

typedef struct{
  unsigned int identifier;
  char* name;

  NetworkNamesapce source;
  NetworkNamesapce destination;

  // Veth* paired_veth;
  int status;
}Veth;

typedef struct {
  unsigned int identifier;
  char* name;

  NetworkNamesapce net_namespace;

  Veth* veth_pairs;
  int current_veth_number;

  int status;
}Bridge;

typedef struct {
  NetworkNamesapce* net_namespace;
  int packet_number_persecond;
}ThreadArgument_Speicified_Packet_Number_Per_Second;

typedef struct {
  NetworkNamesapce* net_namespace;
  int simulation_time;
}ThreadArgument_Trafffic_Generation;


NetworkNamesapce announceNetworkNamesapce_general(unsigned int identifier){
  NetworkNamesapce net_namespace;
  net_namespace.identifier = identifier;

  char name[MAX_NAME_LENGTH];
  sprintf(name, "ns%d", identifier);
  net_namespace.name = malloc(strlen(name) + 1); 
  if (net_namespace.name == NULL) {
      perror("malloc failed");
      exit(EXIT_FAILURE);
  }
  strcpy(net_namespace.name, name); 

  char ip_address[MAX_IP_LENGTH];
  sprintf(ip_address, "10.0.0.%d", identifier);
  net_namespace.ip_address = malloc(strlen(ip_address) + 1); 
  if (net_namespace.ip_address == NULL) {
      perror("malloc failed");
      exit(EXIT_FAILURE);
  }
  strcpy(net_namespace.ip_address, ip_address);

  net_namespace.status = STATUS_ANNOUCED;
  
  return net_namespace;
}

Veth announceVeth_general(unsigned int identifier, NetworkNamesapce net_namespace_source, NetworkNamesapce net_namespace_destination){
  Veth veth;

  veth.identifier = identifier;
  char name[MAX_NAME_LENGTH];
  sprintf(name, "veth-%d", identifier);
  veth.name = malloc(strlen(name) + 1); 
  if (veth.name == NULL) {
      perror("malloc failed");
      exit(EXIT_FAILURE);
  }
  strcpy(veth.name, name); 

  veth.source = net_namespace_source;
  veth.destination = net_namespace_destination;

  veth.status = STATUS_ANNOUCED;
  return veth;
}

Bridge announceBridge_general(unsigned int identifier, NetworkNamesapce net_namespace){
  Bridge bridge;
  bridge.identifier = identifier;

  char name[MAX_NAME_LENGTH];
  sprintf(name, "brdg-%s", net_namespace.name);
  bridge.name = malloc(strlen(name) + 1); 
  if (bridge.name == NULL) {
      perror("malloc failed");
      exit(EXIT_FAILURE);
  }
  strcpy(bridge.name, name); 

  bridge.net_namespace = net_namespace;

  bridge.veth_pairs = (Veth*)malloc(sizeof(Veth)*MAX_VETH_A_BRIDGE);
  bridge.current_veth_number = 0;

  bridge.status = STATUS_ANNOUCED;
  return bridge;
}

void informNetworkNamesapce(NetworkNamesapce net_namespace){
  printf("ID     : %d\n", net_namespace.identifier);
  printf("Name   : %s\n", net_namespace.name);
  printf("IP     : %s\n", net_namespace.ip_address);
  printf("Status : ");
  switch (net_namespace.status){
    case STATUS_ANNOUCED:
        printf("ANNOUCED\n");
      break;
    case STATUS_CREATED:
        printf("CREATED\n");
      break;
    case STATUS_IP_CONFIGURED:
        printf("IP_CONFIGURED\n");
      break;
    case STATUS_DELETED:
        printf("DELETED\n");
      break;   
    default:
        printf("UNKNOWN\n");
      break;
  }
}

void informVeth(Veth veth){
  printf("ID          : %d\n", veth.identifier);
  printf("Name        : %s\n", veth.name);
  printf("Source      : %s\n", veth.source.name);
  printf("Destination : %s\n", veth.destination.name);
  printf("Status      : ");
  switch (veth.status){
    case STATUS_ANNOUCED:
        printf("ANNOUCED\n");
      break;
    case STATUS_CREATED:
        printf("CREATED\n");
      break;
    case STATUS_IP_CONFIGURED:
        printf("IP_CONFIGURED\n");
      break;
    case STATUS_DELETED:
        printf("DELETED\n");
      break;   
    default:
        printf("UNKNOWN\n");
      break;
  }
}

void informBridge(Bridge bridge){
  printf("ID                : %d\n", bridge.identifier);
  printf("Name              : %s\n", bridge.name);
  printf("Network Namespace : %s\n", bridge.net_namespace.name);
  printf("Current Veth      : %d\n", bridge.current_veth_number);
  for(int index=0;index<bridge.current_veth_number;index++){
    Veth veth = *(bridge.veth_pairs+index);
    printf("%d : %s\n", index, veth.name);
  }
}

void createNetworkNamesapce(NetworkNamesapce* ptr_net_namespace){
  char command[MAX_COMMAND_LENGTH];
  sprintf(command, "sudo ip netns add "INFORM_NS_NAME_FORMAT, ptr_net_namespace->name);
  system(command);

  ptr_net_namespace->status = STATUS_CREATED;
}

void createVethPair(Veth* ptr_veth0, Veth* ptr_veth1){
  char command[MAX_COMMAND_LENGTH];
  sprintf(command, "sudo ip link add "INFORM_VETH_NAME_FORMAT" type veth peer name "INFORM_VETH_NAME_FORMAT, ptr_veth0->name, ptr_veth1->name);
  system(command);

  sprintf(command, "sudo ip link set "INFORM_VETH_NAME_FORMAT" netns "INFORM_NS_NAME_FORMAT, ptr_veth0->name, ptr_veth0->source.name);
  system(command);  

  sprintf(command, "sudo ip link set "INFORM_VETH_NAME_FORMAT" netns "INFORM_NS_NAME_FORMAT, ptr_veth1->name, ptr_veth1->source.name);
  system(command);

  ptr_veth0->status = STATUS_CREATED;
  ptr_veth1->status = STATUS_CREATED;
}

void createBridge(Bridge* ptr_bridge){
  char command[MAX_COMMAND_LENGTH];
  sprintf(command, "sudo ip netns exec "INFORM_NS_NAME_FORMAT" ip link add name "INFORM_BRIDGE_NAME_FORMAT" type bridge", ptr_bridge->net_namespace.name, ptr_bridge->name);
  system(command);

  sprintf(command, "sudo ip netns exec "INFORM_NS_NAME_FORMAT" ip link set "INFORM_BRIDGE_NAME_FORMAT" up", ptr_bridge->net_namespace.name, ptr_bridge->name);
  system(command);
  
  ptr_bridge->status = STATUS_CREATED;
}

void addVethtoBridge(Bridge* ptr_bridge, Veth* ptr_veth){
  if(ptr_bridge->status!=STATUS_CREATED){
    printf("Bridge have not been created!\n");
    exit(EXIT_FAILURE);
  }

  if(strcmp(ptr_veth->source.name, ptr_bridge->net_namespace.name)!=0){
    printf("Bridge network namespace and veth source not matched!\n");
    exit(EXIT_FAILURE);    
  }
  
  *(ptr_bridge->veth_pairs + ptr_bridge->current_veth_number) = *ptr_veth;
  ptr_bridge->current_veth_number += 1;

  char command[MAX_COMMAND_LENGTH];
  sprintf(command, "sudo ip netns exec "INFORM_NS_NAME_FORMAT" ip link set "INFORM_VETH_NAME_FORMAT" up ", ptr_bridge->net_namespace.name, ptr_veth->name);
  system(command);

  sprintf(command, "sudo ip netns exec "INFORM_NS_NAME_FORMAT" ip link set "INFORM_VETH_NAME_FORMAT" master "INFORM_BRIDGE_NAME_FORMAT, ptr_bridge->net_namespace.name, ptr_veth->name, ptr_bridge->name);
  system(command);
}

void configureIPaddressNetworkNamesapce_bridge(NetworkNamesapce* ptr_net_namespace, Bridge* ptr_bridge){
  char command[MAX_COMMAND_LENGTH];
  sprintf(command, "sudo ip netns exec "INFORM_NS_NAME_FORMAT" ip addr add "INFORM_IP_ADDRESS_FORMAT"/24 dev "INFORM_BRIDGE_NAME_FORMAT, ptr_net_namespace->name, ptr_net_namespace->ip_address, ptr_bridge->name);
  system(command);

  sprintf(command, "sudo ip netns exec "INFORM_NS_NAME_FORMAT" ip link set "INFORM_BRIDGE_NAME_FORMAT" up", ptr_net_namespace->name, ptr_bridge->name);
  system(command);
  ptr_net_namespace->status = STATUS_IP_CONFIGURED;
}

void configureIPaddressNetworkNamesapce_veth(NetworkNamesapce* ptr_net_namespace, Veth* ptr_veth){
  char command[MAX_COMMAND_LENGTH];
  sprintf(command, "sudo ip netns exec "INFORM_NS_NAME_FORMAT" ip addr add "INFORM_IP_ADDRESS_FORMAT"/24 dev "INFORM_VETH_NAME_FORMAT, ptr_net_namespace->name, ptr_net_namespace->ip_address, ptr_veth->name);
  system(command);

  sprintf(command, "sudo ip netns exec "INFORM_NS_NAME_FORMAT" ip link set "INFORM_VETH_NAME_FORMAT" up", ptr_net_namespace->name, ptr_veth->name);
  system(command);

  ptr_net_namespace->status = STATUS_IP_CONFIGURED;
}

void linkSetUp(NetworkNamesapce net_namespace){
  char command[MAX_COMMAND_LENGTH];
  sprintf(command, "sudo ip netns exec "INFORM_NS_NAME_FORMAT" ip link set veth-"INFORM_NS_NAME_FORMAT" up", net_namespace.name, net_namespace.name);
  system(command); 
}

void deleteNetworkNamesapce(NetworkNamesapce* ptr_net_namespace){
  char command[MAX_COMMAND_LENGTH];
  sprintf(command, "sudo ip netns delete "INFORM_NS_NAME_FORMAT, ptr_net_namespace->name);
  system(command); 

  ptr_net_namespace->status = STATUS_DELETED; 
}

void deleteVeth(Veth* ptr_veth){
  char command[MAX_COMMAND_LENGTH];
  sprintf(command, "sudo ip link delete "INFORM_VETH_NAME_FORMAT, ptr_veth->name);
  system(command); 

  ptr_veth->status = STATUS_DELETED; 
}

void executeReceiveProgram(NetworkNamesapce* ptr_net_namespace){
  char command[MAX_COMMAND_LENGTH];
  sprintf(command, "sudo ip netns exec "INFORM_NS_NAME_FORMAT" gcc ./"RECEIVE_PROGRAM".c -o ../Ausführung/"RECEIVE_PROGRAM, ptr_net_namespace->name);
  system(command);  
  // printf("%s\n", command);

  sprintf(command, "sudo ip netns exec "INFORM_NS_NAME_FORMAT" ../Ausführung/"RECEIVE_PROGRAM, ptr_net_namespace->name);
  system(command);  
  // printf("%s\n", command);
}

void* receiveThread(void* argument) {
  // pid_t pid = getpid();
  // pthread_t tid = pthread_self();
  // fflush(stdout);
  // printf("PID: %d, TID: %lu\n", (int)pid, (unsigned long)tid);
  NetworkNamesapce* net_namespace = (NetworkNamesapce*)argument;
  executeReceiveProgram(net_namespace);
  return NULL;
}

void executeSendProgram(NetworkNamesapce* ptr_source_net_namespace){
  char command[MAX_COMMAND_LENGTH];
  sprintf(command, "sudo ip netns exec "INFORM_NS_NAME_FORMAT" gcc ./"SEND_PROGRAM".c -o ../Ausführung/"SEND_PROGRAM, ptr_source_net_namespace->name);
  system(command);  

  sprintf(command, "sudo ip netns exec "INFORM_NS_NAME_FORMAT" ../Ausführung/"SEND_PROGRAM" "INFORM_IP_ADDRESS_FORMAT" "DESTINATION_IP" %d", ptr_source_net_namespace->name, ptr_source_net_namespace->ip_address, ptr_source_net_namespace->identifier);
  system(command); 
  // printf("%s\n", command); 
}

void executeSendProgramSpeiciedPersencond(NetworkNamesapce* ptr_source_net_namespace, int packet_number_persecond){

  char command[MAX_COMMAND_LENGTH];
  sprintf(command, "sudo ip netns exec "INFORM_NS_NAME_FORMAT" gcc ./"SEND_PROGRAM_SPECIEFIED_PACKET_NUMBER_PER_SECOND".c -o ../Ausführung/"SEND_PROGRAM_SPECIEFIED_PACKET_NUMBER_PER_SECOND, ptr_source_net_namespace->name);
  system(command);  

  sprintf(command, "sudo ip netns exec "INFORM_NS_NAME_FORMAT" ../Ausführung/"SEND_PROGRAM_SPECIEFIED_PACKET_NUMBER_PER_SECOND" "INFORM_IP_ADDRESS_FORMAT" "DESTINATION_IP" %d %d", ptr_source_net_namespace->name, ptr_source_net_namespace->ip_address, ptr_source_net_namespace->identifier, packet_number_persecond);
  system(command); 

}

void executeSendProgramTrafficGeneration(NetworkNamesapce* ptr_source_net_namespace, int simulation_time){
  char command[MAX_COMMAND_LENGTH];
  sprintf(command, "sudo ip netns exec "INFORM_NS_NAME_FORMAT" gcc ./"SEND_PROGRAM_TRAFFIC_GENERATION".c -o ../Ausführung/"SEND_PROGRAM_TRAFFIC_GENERATION, ptr_source_net_namespace->name);
  system(command);  

  sprintf(command, "sudo ip netns exec "INFORM_NS_NAME_FORMAT" ../Ausführung/"SEND_PROGRAM_TRAFFIC_GENERATION" "INFORM_IP_ADDRESS_FORMAT" "DESTINATION_IP" %d "INFORM_NS_NAME_FORMAT" %d", ptr_source_net_namespace->name, ptr_source_net_namespace->ip_address, ptr_source_net_namespace->identifier, ptr_source_net_namespace->name, simulation_time);
  system(command); 
}


void* sendThread(void* argument) {
  NetworkNamesapce* net_namespace = (NetworkNamesapce*)argument;
  executeSendProgram(net_namespace);
  return NULL;
}

void* sendThreadSpeiciedPersencond(void* argument) {
  ThreadArgument_Speicified_Packet_Number_Per_Second* thread_arguments = (ThreadArgument_Speicified_Packet_Number_Per_Second*)argument;
  struct timespec start_time, end_time;
  long elapsed_time_ns;
  
  executeSendProgramSpeiciedPersencond(thread_arguments->net_namespace, thread_arguments->packet_number_persecond);
  clock_gettime(CLOCK_REALTIME, &start_time);
  elapsed_time_ns = (end_time.tv_sec - start_time.tv_sec) * ONE_SECOND_IN_NS + (end_time.tv_nsec - start_time.tv_nsec);
  
    if (elapsed_time_ns < ONE_SECOND_IN_NS) {
        struct timespec remaining_time;
        remaining_time.tv_sec = 0;
        remaining_time.tv_nsec = ONE_SECOND_IN_NS - elapsed_time_ns;

        // Sleep for the remaining time to make the total execution time exactly 1 second
        nanosleep(&remaining_time, NULL);
    }
  return NULL;
}

void* sendThreadTrafficGeneration(void* argument) {
  ThreadArgument_Trafffic_Generation* thread_arguments = (ThreadArgument_Trafffic_Generation*)argument;
  printf("name = %s\n", thread_arguments->net_namespace->name);
  executeSendProgramTrafficGeneration(thread_arguments->net_namespace, thread_arguments->simulation_time);
  return NULL;
}
void* sendThreadTrafficGeneration_Multiple(void* argument) {
  ThreadArgument_Trafffic_Generation* thread_arguments = (ThreadArgument_Trafffic_Generation*)argument;
  fflush(stdout);
  printf("name = %s\n", thread_arguments->net_namespace->name);
  int* traffic = generateNormalDistribution(thread_arguments->simulation_time, MEAN, STANDARD_DEVIATION, thread_arguments->net_namespace->identifier);
  for(int time_stamp = 0;time_stamp<thread_arguments->simulation_time;time_stamp++){
    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "sudo ip netns exec "INFORM_NS_NAME_FORMAT" iperf -c "DESTINATION_IP" -u -b %ld -l %d -t 1", thread_arguments->net_namespace->name, PACKET_SIZE*(*(traffic+time_stamp))*8 ,PACKET_SIZE);
    system(command);
  }
  return NULL;
}

#endif


#ifdef CHECK_SUM
unsigned short checksum(void *buf, int len) {    
    unsigned short *buffer = buf;
    unsigned int sum = 0;
    unsigned short result;
 
    for (sum = 0; len > 1; len -= 2)
        sum += *buffer++;
    if (len == 1)
        sum += *(unsigned char*)buffer;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
} 
#endif