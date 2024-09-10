
#include <sys/stat.h>  
#include <unistd.h> 

#define NETZ
#define HTTP_POST_EINZEL "http://140.123.91.94:8000/POST_EINZEL"

#ifdef NETZ

/**
 * @brief Writes data to a file stream.
 *
 * This function is a wrapper around the `fwrite()` function. It writes data from the
 * provided pointer to the specified file stream. The function writes `nmemb` items
 * of size `größe` from the memory pointed to by `zeiger` into the file stream `strom`.
 *
 * @param zeiger Pointer to the data to be written.
 * @param größe Size of each element to be written, in bytes.
 * @param nmemb Number of elements to write.
 * @param strom Pointer to the file stream where data is written.
 *
 * @return The number of elements successfully written. This should be equal to `nmemb`
 *         if the operation is successful. If an error occurs, a value less than `nmemb`
 *         is returned.
 */
size_t rückruf_schreiben(void *zeiger, size_t größe, size_t nmemb, FILE *strom) {
    return fwrite(zeiger, größe, nmemb, strom);
}

/**
 * @brief Read speicified file and return the content
 * 
 * @param DATEI_NAME The file path
 * @return The content of the file.
 */
char *lesenDatei(char *DATEI_NAME) {
    FILE *datei = fopen(DATEI_NAME, "rb");
    if (!datei) {
        fprintf(stderr, KRED_L"Datei konnte nicht geöffnet werden: %s\n"RESET, DATEI_NAME);
		exit(FEHLGESCHLAGEN);
    }

    fseek(datei, 0, SEEK_END);
    long dateigröße = ftell(datei);
    fseek(datei, 0, SEEK_SET);

#ifdef DRUCKEN_DATEI_LÄNGE
	printf("Datei [%30s] mit Größe %10ld\n", DATEI_NAME, dateigröße);
#endif

    char *puffer = (char *)malloc(dateigröße + 1);
    if (!puffer) {
        fprintf(stderr, KRED_L"Speicherzuordnung fehlgeschlagen\n"RESET);
        fclose(datei);
        exit(FEHLGESCHLAGEN);
    }

    fread(puffer, 1, dateigröße, datei);
    puffer[dateigröße] = '\0';

    fclose(datei);
    return puffer;
}


/**
 * @brief Send HTTP request to solve CPLEX LP problem from the CPLEX Server
 * 
 * This function read the LP file created in the Haupt, send file to CPLEX Server and return the solution stored in Lösung folder.
 * 
 * @param streit The struct of the topology
 * @return .
 */
void pfostenHaupt(struct Streit streit){
	CURL *curl;
	CURLcode resonanz;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Lineare Programmdatei lesen
		char lineareProgrammdateiName[MAX_NAME_LÄNGE];
	    sprintf(lineareProgrammdateiName, "../Datei/Haupt_%s.lp", streit.DATEI_NAME);
        printf("%s\n", lineareProgrammdateiName);

        char *dateiinhalt = lesenDatei(lineareProgrammdateiName);
        if (!dateiinhalt) {
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            exit(FEHLGESCHLAGEN);
        }

#ifdef DRUCKEN_DATEI_INHALT
        printf("Datei [%s] :\n", lineareProgrammdateiName);
        printf("%s\n", dateiinhalt);
#endif

        // Nutzlasten
        char nutzlast[MAXIMALE_NUTZLASTGRÖßE];
        snprintf(nutzlast, sizeof(nutzlast), "%s", dateiinhalt);
        free(dateiinhalt);

        // Festlegen der URL für die POST-Anforderung
        curl_easy_setopt(curl, CURLOPT_URL, HTTP_POST_EINZEL);

        // Setzen Sie den Content-Type-Header auf application/json
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Festlegen der JSON-Nutzlast für die POST-Anforderung
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, nutzlast);

		// In die Lösungsdatei schreiben
		char lösungsdateiName[MAX_NAME_LÄNGE];
        sprintf(lösungsdateiName, "../Lösung/Haupt_%s.sol", streit.DATEI_NAME);

        FILE *lösungsdatei = fopen(lösungsdateiName, "wb"); // Datei im Binärschreibmodus öffnen
        if (lösungsdatei) {
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, rückruf_schreiben);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, lösungsdatei);
            
            // Führen Sie die POST-Anfrage aus
            resonanz = curl_easy_perform(curl);
            
            // Auf Fehler prüfen
            if (resonanz != CURLE_OK) 
                fprintf(stderr, KRED_L"curl_easy_perform() ist fehlgeschlagend: %s\n"RESET, curl_easy_strerror(resonanz));
            
            // Schließen Sie die Datei
            fclose(lösungsdatei);

#ifdef DRUCKEN_LÖSUNG_ERSTELLTMODELLFORTSCHRITT
            printf("Lösung gespeichert in %s\n", lösungsdateiName);
#endif
        }else printf("Datei konnte nicht zum Schreiben geöffnet werden.\n");

        // Aufräumen
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

#endif