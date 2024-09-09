#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ERFOLG 0
#define FEHLGESCHLAGEN -1
#define RICHTIG 1
#define FALSCH -1

#define DRUCKEN_MODELL "%18s"
#define DRUCKEN_INTEGER "%3d"
#define DRUCKEN_DOUBLE  "%12.7lf"
#define DRUCKEN_DATEI_NAME "%10s"

#define ANZEIGEN 1
#define AUSBLENDEN 0

#define MAX_NAME_LÄNGE 1024
#define MAX_BEFEHL_LÄNGE 4096
#define MAX_PUFFER_LÄNGE 4096
#define MAXIMALE_NUTZLASTGRÖßE 1048576

#ifndef _DEBUG_COLOR_
#define _DEBUG_COLOR_
    #define KDRK "\x1B[0;30m"
    #define KGRY "\x1B[1;30m"
    #define KRED "\x1B[0;31m"
    #define KRED_L "\x1B[1;31m"
    #define KGRN "\x1B[0;32m"
    #define KGRN_L "\x1B[1;32m"
    #define KYEL "\x1B[0;33m"
    #define KYEL_L "\x1B[1;33m"
    #define KBLU "\x1B[0;34m"
    #define KBLU_L "\x1B[1;34m"
    #define KMAG "\x1B[0;35m"
    #define KMAG_L "\x1B[1;35m"
    #define KCYN "\x1B[0;36m"
    #define KCYN_L "\x1B[1;36m"
    #define WHITE "\x1B[0;37m"
    #define WHITE_L "\x1B[1;37m"
    #define RESET "\x1B[0m"
#endif
