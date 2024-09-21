#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SUCCESS 0
#define FAILED -1
#define UNFOUND -1

#define INFORM_IDENTIFIER_FORMAT    "%03u"
#define INFORM_TRAFFIC_FORMAT       "%12.7lf"
#define INFORM_DOUBLE_FORMAT        "%12.7lf"
#define INFORM_TIME_FORMAT          "%6d"

#define MAX_COMMAND_LENGTH  1024
#define GENERATE_BINARY (int)(rand()%2) /* @brief Generate a binary random value (0 or 1) */ 