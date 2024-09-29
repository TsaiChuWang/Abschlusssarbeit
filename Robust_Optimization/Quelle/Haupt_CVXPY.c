
#include <curl/curl.h>	// sudo apt-get install libcurl4-openssl-dev

#include "../Enthalten/Konfiguration.h"
#include "../Enthalten/Aufbau.h"
#include "../Enthalten/Streit.h"
#include "../Enthalten/Netz.h"
#include "../Enthalten/Guassian.h"
#include "../Enthalten/Parser.h"

#define MATLAB_SKRIPT_WEG "../Quelle/Haupt_CVXPY.m"

// gcc ./Haupt_CVXPY.c -o ../Ausführung/Haupt_CVXPY -lcurl -lgsl -lgslcblas -lm
// ../Ausführung/Haupt_CVXPY USA-26 
// ../Ausführung/Haupt_CVXPY OR 
// ../Ausführung/Haupt_CVXPY MINTOPO3ENANTS

int main(int argc, char *argv[]){
	// Drucken Sie die Streiten und Fehlgeschlagen
	if(argc<2){
		printf(KRED_L"FEHLER: FEHLT ARGUMENTE\n"RESET);
		return FEHLGESCHLAGEN;
	}

    struct Streit streit = nehmenStreit(zuordnungCode(argv[1]));
	
	char* DATEI_NAME = streit.DATEI_NAME;
	int ANZAHL_KANTEN = streit.ANZAHL_KANTEN;
	int ANZAHL_KNOTEN = streit.ANZAHL_KNOTEN;

#ifdef ALLE_GLEICHE_KAPAZITÄT
	double KAPAZITÄT = streit.GLEICHE_KAPAZITÄT;
#else
    printf(KRED_L"FEHLER: FEHLT DIESES MODELL IST NOCH NICHT VOLLSTÄNDIG!\n"RESET);
	return FEHLGESCHLAGEN;
#endif

    int** BOGEN= streit.BOGEN;
	int** LISTE = streit.LISTE;

    // GRENZE_VERKHERSEICHWITE 
    switch(MODUS_GRENZE_VERKHERSEICHWITE)
    {
        case INDEX_BEGRENZT:
            erhaltenGrenzeVerkhersreichwite_INDEX_BEGRENZT(&streit);
            break;
        case ZUFÄLLIGE_MISCHUNG:
            erhaltenGrenzeVerkhersreichwite_ZUFÄLLIGE_MISCHUNG(&streit);
            break;
        case STABILWERT:
            erhaltenGrenzeVerkhersreichwite_STABILWERT(&streit, 1,10);
            break;
        case STABILGRENZE:
            // erhaltenGrenzeVerkhersreichwite_STABILGRENZE(&streit);
            break;
        default:
            erhaltenGrenzeVerkhersreichwite_INDEX_BEGRENZT(&streit);
            break;
    }
    double*** GRENZE_VERKHERSEICHWITE = streit.GRENZE_VERKHERSEICHWITE;

#ifdef STREIT_DRUCKEN
	printf("Topology : %s [%d]\n", argv[1], zuordnungCode(argv[1]));
    printf("Knoten   : "DRUCKEN_INTEGER" x "DRUCKEN_INTEGER", Kanten : "DRUCKEN_INTEGER" \n", ANZAHL_KNOTEN, ANZAHL_KNOTEN, ANZAHL_KANTEN);

#ifdef ALLE_GLEICHE_KAPAZITÄT
	printf("[ALLE_GLEICHE_KAPAZITÄT] Kapazität : "DRUCKEN_DOUBLE"\n", KAPAZITÄT);
#else
    printf(KRED_L"FEHLER: FEHLT DIESES MODELL IST NOCH NICHT VOLLSTÄNDIG!\n"RESET);
	return FEHLGESCHLAGEN;
#endif
    printf("TOPOLOGY : \n");
    druckenTopology(streit);
    printf("\n VERKEHR GRENZE : \n");
    druckenGrenzeVerkhersreichwite(streit);
#endif

    // Scalter
    int einschränkungszähler = 1;
    int schalter = 0;
    int quadratischschalter =1;

    // Konstante
    double** mu_bedeutung = erhaltenBedeutung(streit);
    double** sigma_kovarianz = erhaltenKovarianz(streit);

    druckenGrenzeVerkhersreichwite(streit);
    
    free(BOGEN);
    free(LISTE);
    free(mu_bedeutung);
    

    for(int quelle_index=0;quelle_index<streit.ANZAHL_KNOTEN;quelle_index++){
        free(GRENZE_VERKHERSEICHWITE[0][quelle_index]);
        free(GRENZE_VERKHERSEICHWITE[1][quelle_index]);
    }
    
    free(GRENZE_VERKHERSEICHWITE[0]);
    free(GRENZE_VERKHERSEICHWITE[1]);

    free(GRENZE_VERKHERSEICHWITE);
    
    return ERFOLG;
}