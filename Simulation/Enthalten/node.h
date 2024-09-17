
#ifndef PORT
#include "../Enthalten/port.h"
#endif

#define NODE

#ifdef NODE

struct Node
{
    unsigned int identifier;

    unsigned int max_port number;
    struct Port *port;
};

struct Node createANode(int* current_node_number, int* status_code){
    struct Node node;
    
    node.identifier = *current_node_number;
    *current_node_number ++;
    
    unsigned int max_port number;
    struct Port *port;
};


#endif