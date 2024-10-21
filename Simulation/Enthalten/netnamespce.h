#define _NET_NAMESPCE

#include "../Enthalten/include.h"

#ifdef _NET_NAMESPCE

#define STATUS_ANNOUCED 0
#define STATUS_CREATED  1
#define STATUS_DELETED  2

unsigned short checksum(void *buf, int len) {    
    unsigned short *buffer = buf;
    unsigned int sum = 0;
    unsigned short result;
 
    for (sum = 0; len > 1; len -= 2)
        sum += *uffer++;
    if (len == 1)
        sum += *(unsigned char*)uffer;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
} 

typedef struct{
  unsigned int identifier;
  char* name;
  char* ip_address;

  int status;
}NetworkNamesapce;

NetworkNamesapce announceNetworkNamesapce_general(unsigned int identifier){
  NetworkNamesapce net_namespace;
  net_namespace.identifier = identifier;

  char name[MAX_NAME_LENGTH];
  sprintf(name, "ns%d", identifier);
  net_namespace.name = name;

  char ip_address[MAX_IP_LENGTH];
  sprintf(ip_address, "10.0.0.%d", identifier);
  net_namespace.ip_address = ip_address;

  net_namespace.status = STATUS_ANNOUCED;

  return net_namespace;
}

void createNetworkNamesapce(NetworkNamesapce net_namespace){
  char command[MAX_COMMAND_LENGTH];
  sprintf(command, "sudo ip netns add %s", net_namespace.name);
  system(command);

  net_namespace.status = STATUS_CREATED;
}

void addVethPair(NetworkNamesapce net_namespace_source, NetworkNamesapce net_namespace_destination, int* pair_identifier){
  char command[MAX_COMMAND_LENGTH];
  sprintf(command, "sudo ip link add veth%d type veth peer name veth-br%d", *pair_identifier, *pair_identifier);
  system(command);  
  (*pair_identifier) +=1;
}

void deleteNetworkNamesapce(NetworkNamesapce net_namespace){
  char command[MAX_COMMAND_LENGTH];
  sprintf(command, "sudo ip netns delete %s", net_namespace.name);
  system(command);  

  net_namespace.status = STATUS_DELETED; 
}

#endif