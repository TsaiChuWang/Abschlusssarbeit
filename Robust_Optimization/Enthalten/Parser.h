
#define PARSER

#define PROGRAM_NAME            "./Parser.py"
#define TEMPERATUR_DATEI_WEG    "../Datei/Parser_Temperatur.txt"

#ifdef PARSER

/**
 * @brief Computes the value of alpha based on the result from an external Python script.
 * 
 * This function executes a Python script that processes a data file related to the input 
 * structure `streit` which extract the alpha value form the specified solution file a from the output. The script is executed 
 * using a system call, and the result is read from a temporary file.
 * 
 * @param streit The input structure containing information about the problem, streit.DATEI_NAME is the parameter of the file name used by the Python script.
 * 
 * @return The computed value of \a alpha. If the process fails, a default value 
 * `FEHLGESCHLAGEN` is returned.
 * 
 * @note If the file cannot be opened, the function will terminate the program with an error message.
 */

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