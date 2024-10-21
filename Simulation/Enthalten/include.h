#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SUCCESS 0
#define FAILED -1
#define UNFOUND -1
#define COMPARE_TRUE 0

#define TRUE 1
#define FALSE -1

#define INFORM_IDENTIFIER_FORMAT    "%03u"
#define INFORM_TRAFFIC_FORMAT       "%12.7lf"
#define INFORM_DOUBLE_FORMAT        "%12.7lf"
#define INFORM_TIME_FORMAT          "%6d"

#define MAX_COMMAND_LENGTH  1024
#define MAX_NAME_LENGTH     512
#define MAX_IP_LENGTH       16
#define MAX_PATH_LENGTH     2048
#define MAX_BUFFER_SIZE     4096
#define GENERATE_BINARY (int)(rand()%2) /* @brief Generate a binary random value (0 or 1) */ 

#define INFORM_IP_ADDRESS_FORMAT  "%16s"
#define INFORM_PORT_FORMAT        "%6d"
#define INFORM_PACKET_SIZE_FORMAT "%6d"
#define INFORM_NS_NAME_FORMAT     "%s"
#define INFORM_VETH_NAME_FORMAT   "%s"
#define INFORM_BRIDGE_NAME_FORMAT "%s"