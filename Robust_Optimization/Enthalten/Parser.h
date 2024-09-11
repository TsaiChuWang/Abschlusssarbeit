
#define PARSER

#define PROGRAM_NAME            "./Parser.py"
#define TEMPERATUR_DATEI_WEG    "../Datei/Parser_Temperatur.txt"

#ifdef PARSER

double erhaltenALPHA(struct Streit streit){
    double alpha = FEHLGESCHLAGEN;
    
    char * linie = NULL;
    size_t länge = 0;
    ssize_t read;

    char befehl[MAX_BEFEHL_LÄNGE];
    sprintf(befehl, "python3 "PROGRAM_NAME" 1 %s\n", streit.DATEI_NAME);
    system(befehl);

    FILE *dateizeiger = fopen(TEMPERATUR_DATEI_WEG "r"); 
    if (dateizeiger == NULL) {
        printf("Fehler: Die Datei konnte nicht geöffnet werden.\n"); 
        exit(EXIT_FAILURE);
    }
 
    double nu = 0;
    while ((read = getline(&linie, &länge, dateizeiger)) != FEHLGESCHLAGEN)
        alpha = strtod(linie, NULL);

    fclose(dateizeiger);
    
#ifdef DRUCKEN_ERHALTET_ALPHA
    printf("alpha = "DRUCKEN_DOUBLE"\n", alpha);
#endif

    return alpha;
}
#endif