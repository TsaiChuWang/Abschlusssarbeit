
#include <curl/curl.h>	// sudo apt-get install libcurl4-openssl-dev

#include "../Enthalten/Konfiguration.h"
#include "../Enthalten/Aufbau.h"
#include "../Enthalten/Streit.h"
#include "../Enthalten/Netz.h"
#include "../Enthalten/Guassian.h"
#include "../Enthalten/Parser.h"

// gcc ./Haupt_CPLEX.c -o ../Ausführung/Haupt_CPLEX -lcurl -lgsl -lgslcblas -lm
// ../Ausführung/Haupt_CPLEX USA-26

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

    // Öffnen Sie die Textdatei
	FILE *dateizeiger;
	char datei_name[MAX_NAME_LÄNGE];
	sprintf(datei_name, "../Datei/Haupt_%s.lp", DATEI_NAME);
    dateizeiger = fopen(datei_name,"w+");

    double umgekehrt_Gaussian = gsl_cdf_gaussian_Pinv(ETA, 1.0);
#ifdef DRUCKEN_ETA_MIT_UMGEKEHRT_GAUSSIAN
    printf("Φ^{-1}(η) = "DRUCKEN_DOUBLE"\n", umgekehrt_Gaussian);
#endif
    // min ɑ
    fprintf(dateizeiger, "Minimize\n");
    fprintf(dateizeiger, "\tobj: "OBJEKTIV"\n");
    
    fprintf(dateizeiger, "\nSubject to \n\n");

    // \sum_{s,t\in N, s\neq t}\textcolor{blue}{f_{st}(e)}\mu_{st}  = \delta_e & \forall e\in E
    for(int kant=1;kant<=ANZAHL_KANTEN;kant++){
        fprintf(dateizeiger, DRUCKEN_ZWANG_ZAHL, einschränkungszähler);
        for(int quelle=1;quelle<=ANZAHL_KNOTEN;quelle++)
            for(int ziel=1;ziel<=ANZAHL_KNOTEN;ziel++)
                if(quelle!=ziel)
                    if(quelle == ANZAHL_KNOTEN && ziel == ANZAHL_KNOTEN-1)
                        fprintf(dateizeiger, DRUCKEN_DOUBLE" "PSD_VARIABLE" ", mu_bedeutung[quelle-1][ziel-1], quelle, ziel, kant);
                    else fprintf(dateizeiger, DRUCKEN_DOUBLE" "PSD_VARIABLE" + ", mu_bedeutung[quelle-1][ziel-1], quelle, ziel, kant);

#ifdef ALLE_GLEICHE_KAPAZITÄT  
        fprintf(dateizeiger, " - "DELTA_VARIABLE" = "DRUCKEN_DOUBLE"\n", kant, KAPAZITÄT);
#else
        printf(KRED_L"FEHLER: FEHLT DIESES MODELL IST NOCH NICHT VOLLSTÄNDIG!\n"RESET);
        return FEHLGESCHLAGEN;
#endif
        einschränkungszähler++;
    }
    fprintf(dateizeiger, "\n");

    // [\sum_{s,t\in N, s\neq t}\Phi^{-1}(\eta)^2\sigma_{st}^2\textcolor{blue}{f_{st}(e)}^2-\delta_e^2]+2c_e\delta_e\leq c_e^2 & \forall e\in E
    for(int kant=1;kant<=ANZAHL_KANTEN;kant++){
        fprintf(dateizeiger, DRUCKEN_QUADRATISCH_ZWANG_ZAHL, quadratischschalter);
        for(int quelle=1;quelle<=ANZAHL_KNOTEN;quelle++)
            for(int ziel=1;ziel<=ANZAHL_KNOTEN;ziel++)
                if(quelle!=ziel){
                    double koeffizient = (umgekehrt_Gaussian*umgekehrt_Gaussian*sigma_kovarianz[quelle-1][ziel-1]*sigma_kovarianz[quelle-1][ziel-1]);
                    if(koeffizient>=0.0 && !(quelle==1 && ziel==2))
                        fprintf(dateizeiger, "+ "DRUCKEN_DOUBLE" "PSD_VARIABLE"^2 ", koeffizient, quelle, ziel, kant);
                    else fprintf(dateizeiger, DRUCKEN_DOUBLE" "PSD_VARIABLE"^2 ", koeffizient, quelle, ziel, kant);

#ifdef DRUCKEN_WAHRSCHEINLICHKEITSKOEFFIZIENT
                    printf("Für OD-Parr "DRUCKEN_INTEGER" ~ "DRUCKEN_INTEGER", koeffiient ist "DRUCKEN_DOUBLE"= umgekehrt_Gaussian("DRUCKEN_DOUBLE")^2*sigma("DRUCKEN_DOUBLE")^2\n", quelle, ziel, koeffizient, umgekehrt_Gaussian, sigma_kovarianz[quelle-1][ziel-1]);
#endif

                    if(quelle==ANZAHL_KNOTEN && ziel == ANZAHL_KNOTEN-1)
                        fprintf(dateizeiger, " - "DELTA_VARIABLE"^2 ] ", kant);
                }

        quadratischschalter++;

#ifdef ALLE_GLEICHE_KAPAZITÄT
        fprintf(dateizeiger, " <= 0\n");
#else
        printf(KRED_L"FEHLER: FEHLT DIESES MODELL IST NOCH NICHT VOLLSTÄNDIG!\n"RESET);
        return FEHLGESCHLAGEN;
#endif
    }
    fprintf(dateizeiger, "\n");

    // \sum_{s,t\in N, s\neq t}\textcolor{blue}{f_{st}(e)}\leq \textcolor{blue}\alpha c_e & \forall e\in E
    for(int kant=1;kant<=ANZAHL_KANTEN;kant++){
        fprintf(dateizeiger, DRUCKEN_ZWANG_ZAHL, einschränkungszähler);
        for(int quelle=1;quelle<=ANZAHL_KNOTEN;quelle++)
            for(int ziel=1;ziel<=ANZAHL_KNOTEN;ziel++)
                if(quelle!=ziel){
                    fprintf(dateizeiger, PSD_VARIABLE, quelle, ziel, kant);
                    if(quelle==ANZAHL_KNOTEN && ziel == ANZAHL_KNOTEN-1)
                        continue;
                    else fprintf(dateizeiger, " + ");
                }
        einschränkungszähler ++;
#ifdef ALLE_GLEICHE_KAPAZITÄT
        fprintf(dateizeiger, " - "DRUCKEN_DOUBLE OBJEKTIV" <= 0\n", KAPAZITÄT);
#else
        printf(KRED_L"FEHLER: FEHLT DIESES MODELL IST NOCH NICHT VOLLSTÄNDIG!\n"RESET);
        return FEHLGESCHLAGEN;
#endif
    }
    fprintf(dateizeiger, "\n");
    
    // Flow conservation : constraints
    schalter = 0;
    for(int knote=1;knote<=ANZAHL_KNOTEN;knote++)
        for(int quelle=1;quelle<=ANZAHL_KNOTEN;quelle++)
            for(int ziel=1;ziel<=ANZAHL_KNOTEN;ziel++)
                if(quelle!=ziel){
                    fprintf(dateizeiger, DRUCKEN_ZWANG_ZAHL, einschränkungszähler);
                    for(int kant_index=0;kant_index<ANZAHL_KANTEN;kant_index++){
                        if(suchenHeaderKnoten(streit,kant_index)==knote-1){
                            if(schalter == 0)
                                schalter = 1;
                            else if(schalter == 1)
                                fprintf(dateizeiger, " + ");
                            
                            fprintf(dateizeiger, " "PSD_VARIABLE, quelle, ziel, kant_index+1);
                        }

                        if(suchenSchwanzKnoten(streit,kant_index)==knote-1){
                            if(schalter == 0)
                                schalter = 1;
                            
                            fprintf(dateizeiger, " - ");
                            fprintf(dateizeiger, " "PSD_VARIABLE, quelle, ziel, kant_index+1);
                        } 
                    }

                    if(knote==quelle)
                        fprintf(dateizeiger, " =  "DRUCKEN_DOUBLE"\n", mu_bedeutung[quelle-1][ziel-1]);
                    else if(knote==ziel)
                        fprintf(dateizeiger, " = -"DRUCKEN_DOUBLE"\n", mu_bedeutung[quelle-1][ziel-1]);
                    else  fprintf(dateizeiger, " = 0\n");
                    
                    schalter = 0;
                    einschränkungszähler++;
                }
    fprintf(dateizeiger, "\n");
	
    fprintf(dateizeiger, "\nBounds\n\n"); 

    // \textcolor{blue}{f_{st}(e)}\geq0 & \forall s,t\in N\ s\neq t\ ,e\in E
    for(int kant=1;kant<=ANZAHL_KANTEN;kant++)
        for(int quelle=1;quelle<=ANZAHL_KNOTEN;quelle++)
            for(int ziel=1;ziel<=ANZAHL_KNOTEN;ziel++)
                if(quelle!=ziel)
                    fprintf(dateizeiger, "\t"PSD_VARIABLE" >= 0\n", quelle, ziel, kant);
    fprintf(dateizeiger, "\n");

    for(int kant=1;kant<=ANZAHL_KANTEN;kant++)
        fprintf(dateizeiger, "\t"DELTA_VARIABLE" >= 0\n", kant);

    fprintf(dateizeiger, "\t"OBJEKTIV" >= 0\n");

    fprintf(dateizeiger, "\nEnd\n");
    fclose(dateizeiger);

    // Erhalten Sie die Lösung
    pfostenHaupt(streit);

    // Analysieren
    double alpha = erhaltenALPHA(streit);
    double*** bruchsarray = erhaltenPSDBurch(streit);

    // Speicher freigeben

    for(int index=0;index<ANZAHL_KNOTEN;index++){
        free(*(mu_bedeutung+index));
        free(*(BOGEN+index));
        free(*(LISTE+index));
    }
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