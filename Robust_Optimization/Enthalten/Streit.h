#define STREIT

#include <time.h>
#include "./Aufbau.h"

#define MAXIMALE_TOPOLOGIE_ZÄHL 46

// Modus auf grenze der Verkhersreichwite
// 0 : With index : min(i,j) <= d_{ij} <= max(i,j)
// 1 : With random : median - min(i,j)*random*schrittlänge <= d_{ij} <= maedian + max(i,j)*random*schrittlänge
// 2 : Fixed value : untere grenze der Verkhersreichwite <= d_{ij} <= obere grenze der Verkhersreichwite
// 3 : Fixed range : untere grenze der Verkhersreichwite_{ij} <= d_{ij} <= obere grenze der Verkhersreichwite_{ij}

#define INDEX_BEGRENZT 0
#define ZUFÄLLIGE_MISCHUNG 1
#define STABILWERT 2
#define STABILGRENZE 3

#define MINIMALE_ZUFÄLLIGER_BEREICH 0.8
#define MAXIMALE_ZUFÄLLIGER_BEREICH 1.2

// Modus des Streit
// #define ALLE_GLEICHE_KAPAZITÄT
// #define KANT_UNTERSCHIEDLICHE_KAPAZITÄT
// #define MODUS_GRENZE_VERKHERSEICHWITE 

#ifdef STREIT

struct Streit{
    char* DATEI_NAME;   // File nmae of .lp

    int ANZAHL_KANTEN;  // Number of edges
    int ANZAHL_KNOTEN;  // Number of edges

    double GLEICHE_KAPAZITÄT; // Each link has same capacity 
	double* UNTERSCHIEDLICHE_KAPAZITÄT; // Each liink has different capacity

    int** BOGEN;	// Topology
    int** LISTE;
	int SYMMETRISCH;	// Directed or not

	double*** GRENZE_VERKHERSEICHWITE;	
};

// Interner Anruf

int** bereichkopieren(int anzahl_knoten, int topology[anzahl_knoten][anzahl_knoten]){
    int** bereich = (int**)malloc(sizeof(int*)*anzahl_knoten);
    for(int index=0;index<anzahl_knoten;index++){
        bereich[index] = (int*)malloc(sizeof(int)*anzahl_knoten);
        for(int jndex=0;jndex<anzahl_knoten;jndex++)
            bereich[index][jndex] = topology[index][jndex];
    }
    return bereich;
}

double*** initialisierenGrenzeVerkherseichwite(int ANZAHL_KNOTEN){
	double*** GRENZE_VERKHERSEICHWITE = (double***)calloc(2, sizeof(double**));
	for(int index=0;index<2;index++){
		*(GRENZE_VERKHERSEICHWITE+index) = (double**)calloc(ANZAHL_KNOTEN, sizeof(double*));
		for(int quelle_index = 0;quelle_index<ANZAHL_KNOTEN;quelle_index++)
			*(*(GRENZE_VERKHERSEICHWITE+index)+quelle_index) = (double*)calloc(ANZAHL_KNOTEN, sizeof(double));
	}
	return GRENZE_VERKHERSEICHWITE;
}

double* initialisierenUnterschiedliche_kapazität(int ANZAHL_KANTEN, double kapazität){
	double* UNTERSCHIEDLICHE_KAPAZITÄT = (double*)calloc(ANZAHL_KANTEN, sizeof(double));

	for(int kant_index=0;kant_index<ANZAHL_KANTEN;kant_index++)
		*(UNTERSCHIEDLICHE_KAPAZITÄT+kant_index) = kapazität;

	return UNTERSCHIEDLICHE_KAPAZITÄT;
}

double berechnenMedianUnterschiedliche_kapazität(struct Streit *streit){
	double median = 0.0;
	for(int kant_index;kant_index<streit->ANZAHL_KANTEN;kant_index++)
		median += streit->UNTERSCHIEDLICHE_KAPAZITÄT[kant_index];
	median /= streit->ANZAHL_KANTEN;
	return median;
}

int MIN_INDEX(int index_eins, int index_zwei){
	return index_eins < index_zwei ? index_eins : index_zwei;
}

int MAX_INDEX(int index_eins, int index_zwei){
	return index_eins > index_zwei ? index_eins : index_zwei;
} 

// Externer Anruf

struct Streit nehmenStreit(int code){
    struct Streit streit;

    switch(code){
		case 0:
			streit.DATEI_NAME = "OR";

			streit.ANZAHL_KANTEN = 12;
			streit.ANZAHL_KNOTEN = 6;

			streit.GLEICHE_KAPAZITÄT = 77;

			int topology_0[6][6] =
				{{0,1,1,0,0,0},
				{1,0,0,1,0,0},
				{1,0,0,0,1,0},
				{0,1,0,0,0,1},
				{0,0,1,0,0,1},
				{0,0,0,1,1,0}};

			streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_0);
			streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_0);

			streit.SYMMETRISCH = RICHTIG;

		break;

        case 1:
            streit.DATEI_NAME = "erste_modul";

            streit.ANZAHL_KANTEN = 20;
            streit.ANZAHL_KNOTEN = 6;

            streit.GLEICHE_KAPAZITÄT = 100;
            
            int topology_1[6][6] =
                {{0,1,1,0,0,0},
                 {1,0,1,1,1,0},
                 {1,1,0,1,1,0},
                 {0,1,1,0,1,1},
                 {0,1,1,1,0,1},
                 {0,0,0,1,1,0}};

            streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_1);
            streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_1);

            streit.SYMMETRISCH = RICHTIG;

		break;

		case 2:
			streit.DATEI_NAME = "NSFNET";

			streit.ANZAHL_KANTEN = 42;
			streit.ANZAHL_KNOTEN = 14;

			streit.GLEICHE_KAPAZITÄT = 150;

			int topology_2[14][14] =
				{{0,0,1,0,0,0,0,0,1,0,0,0,0,1},
				{0,0,1,0,0,0,0,0,1,0,0,0,0,1},
				{1,1,0,0,1,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,1,0,0,0,0,0,0,0,0,1},
				{0,0,1,1,0,1,0,0,0,0,0,0,1,0},
				{0,0,0,0,1,0,1,1,0,0,0,0,0,0},
				{0,0,0,0,0,1,0,1,0,0,0,1,0,0},
				{0,0,0,0,0,1,1,0,0,1,0,0,0,0},
				{1,1,0,0,0,0,0,0,0,0,0,1,0,0},
				{0,0,0,0,0,0,0,1,0,0,1,0,0,1},
				{0,0,0,0,0,0,0,0,0,1,0,0,1,0},
				{0,0,0,0,0,0,1,0,1,0,0,0,1,0},
				{0,0,0,0,1,0,0,0,0,0,1,1,0,0},
				{1,1,0,1,0,0,0,0,0,1,0,0,0,0}};

			streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_2);
			streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_2);

			streit.SYMMETRISCH = RICHTIG;

		break;

		case 3:
			streit.DATEI_NAME = "USA-26";

			streit.ANZAHL_KANTEN = 80;
			streit.ANZAHL_KNOTEN = 26;

			streit.GLEICHE_KAPAZITÄT = 700;

			int topology_3[26][26] =
				{{0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{1,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,1,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
				{0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,1,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,1,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,0,0,0,0,0,1},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1},
				{0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,1,0,0,1,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0}};

			streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_3);
			streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_3);

			streit.SYMMETRISCH = RICHTIG;

		break;

		case 4:
			streit.DATEI_NAME = "USA-28";

			streit.ANZAHL_KANTEN = 90;
			streit.ANZAHL_KNOTEN = 28;

			streit.GLEICHE_KAPAZITÄT = 1500;

			int topology_4[28][28] =
				{{0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{1,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,1,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,1,0,0,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0},
				{0,0,0,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,1,0,0,0,0,1,0,1,1,0,0,0,0,1,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,1,1,1,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,1,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,1,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,1,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,1,1},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,1},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0}};

			streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_4);
			streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_4);

			streit.SYMMETRISCH = RICHTIG;

		break;

		case 5:
			streit.DATEI_NAME = "11NODE_COST239";

			streit.ANZAHL_KANTEN = 52;
			streit.ANZAHL_KNOTEN = 11;

			streit.GLEICHE_KAPAZITÄT = 600;

			int topology_5[11][11] =
				{{0,1,1,1,0,0,0,1,0,0,0},
				{1,0,1,0,1,1,1,0,1,0,0},
				{1,1,0,1,1,0,1,0,0,0,0},
				{1,0,1,0,1,0,0,1,1,0,0},
				{0,1,1,1,0,1,0,0,0,1,0},
				{0,1,0,0,1,0,1,0,0,1,1},
				{0,1,1,0,0,1,0,0,0,0,1},
				{1,0,0,1,0,0,0,0,1,1,0},
				{0,1,0,1,0,0,0,1,0,1,1},
				{0,0,0,0,1,1,0,1,1,0,1},
				{0,0,0,0,0,1,1,0,1,1,0}};

			streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_5);
			streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_5);

			streit.SYMMETRISCH = RICHTIG;

		break;

		case 6:	// Pan-European Network Topology
			streit.DATEI_NAME = "Pan-European";

			streit.ANZAHL_KANTEN = 82;
			streit.ANZAHL_KNOTEN = 28;

			streit.GLEICHE_KAPAZITÄT = 1500;

			int topology_6[28][28] =
				{{0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{1,0,1,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,1,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,1,0,1,0,1,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,1,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
				{0,0,0,0,0,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
				{0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,1,0,1,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,1,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,1},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,1,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0},
				{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0}};

			streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_6);
			streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_6);

			streit.SYMMETRISCH = RICHTIG;

		break;

        case 7:
            streit.DATEI_NAME = "SANReN";

            streit.ANZAHL_KANTEN = 14;
            streit.ANZAHL_KNOTEN = 7;

            streit.GLEICHE_KAPAZITÄT = 462;

            int topology_7[7][7] =
                {{0,1,0,0,0,0,1},
                 {1,0,1,0,0,0,0},
                 {0,1,0,1,0,0,0},
                 {0,0,1,0,1,0,0},
                 {0,0,0,1,0,1,0},
                 {0,0,0,0,1,0,1},
                 {1,0,0,0,0,1,0}};

            streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_7);
            streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_7);

            streit.SYMMETRISCH = RICHTIG;

		break;
        case 8:
            streit.DATEI_NAME = "ViaNetwork";

            streit.ANZAHL_KANTEN = 24;
            streit.ANZAHL_KNOTEN = 9;

            streit.GLEICHE_KAPAZITÄT = 260;

            int topology_8[9][9] =
                {{0,1,1,0,0,0,0,0,0},
                {1,0,1,1,1,1,0,0,0},
                {1,1,0,0,0,0,0,0,0},
                {0,1,0,0,0,1,0,0,0},
                {0,1,0,0,0,1,0,0,1},
                {0,1,0,1,1,0,1,0,0},
                {0,0,0,0,0,1,0,1,0},
                {0,0,0,0,0,0,1,0,1},
                {0,0,0,0,1,0,0,1,0}};

            streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_8);
            streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_8);

            streit.SYMMETRISCH = RICHTIG;

		break;

        case 9:
            streit.DATEI_NAME = "BREN";

            streit.ANZAHL_KANTEN = 22;
            streit.ANZAHL_KNOTEN = 10;

            streit.GLEICHE_KAPAZITÄT = 220;

            int topology_9[10][10] =
                {{0,1,1,0,0,0,0,0,0,0},
                {1,0,0,0,0,0,1,0,0,0},
                {1,0,0,1,0,0,0,0,0,0},
                {0,0,1,0,1,0,0,0,0,0},
                {0,0,0,1,0,1,0,1,0,0},
                {0,0,0,0,1,0,1,0,0,0},
                {0,1,0,0,0,1,0,0,1,0},
                {0,0,0,0,1,0,0,0,0,1},
                {0,0,0,0,0,0,1,0,0,1},
                {0,0,0,0,0,0,0,1,1,0}};

            streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_9);
            streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_9);

            streit.SYMMETRISCH = RICHTIG;

		break;

        case 10:    // Rede Nacional de Ensino e Pesquisa (RNP)
            streit.DATEI_NAME = "RNP";  

            streit.ANZAHL_KANTEN = 26;  // Zahl des Umrand fehlschlagen
            streit.ANZAHL_KNOTEN = 10;

            streit.GLEICHE_KAPAZITÄT = 240;

            int topology_10[10][10] =
                {{0,1,0,1,0,0,0,0,0,0},
                {1,0,1,0,0,0,0,0,0,0},
                {0,1,0,0,1,0,0,0,0,0},
                {1,0,0,0,1,1,0,0,0,0},
                {0,0,1,1,0,0,1,0,0,0},
                {0,0,0,1,0,0,1,1,0,0},
                {0,0,0,0,1,1,0,1,0,0},
                {0,0,0,0,0,1,1,0,1,1},
                {0,0,0,0,0,0,0,1,0,1},
                {0,0,0,0,0,0,0,1,1,0}};

            streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_10);
            streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_10);

            streit.SYMMETRISCH = RICHTIG;

		break;

    case 11:    // Abilene Core
        streit.DATEI_NAME = "AbileneCore";

        streit.ANZAHL_KANTEN = 26;     
        streit.ANZAHL_KNOTEN = 10;

        streit.GLEICHE_KAPAZITÄT = 260;

        int topology_11[10][10] =
            {{0,1,1,0,0,0,0,0,0,0},
            {1,0,0,1,0,0,0,0,0,0},
            {1,0,0,1,1,0,0,0,0,0},
            {0,1,1,0,0,1,0,0,0,0},
            {0,0,1,0,0,1,1,0,0,0},
            {0,0,0,1,1,0,0,0,0,1},
            {0,0,0,0,1,0,0,1,1,0},
            {0,0,0,0,0,0,1,0,1,0},
            {0,0,0,0,0,0,1,1,0,1},
            {0,0,0,0,0,1,0,0,1,0}};

        streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_11);
        streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_11);

        streit.SYMMETRISCH = RICHTIG;

		break;

    case 12:    // LEARN Network
        streit.DATEI_NAME = "LEARN";

        streit.ANZAHL_KANTEN = 22;  // Zahl des Umrand fehlschlagen
        streit.ANZAHL_KNOTEN = 10;

        streit.GLEICHE_KAPAZITÄT = 240;

        int topology_12[10][10] =
            {{0,1,1,0,0,0,0,0,0,0},
            {1,0,0,0,0,1,0,0,0,0},
            {1,0,0,1,0,0,0,0,0,0},
            {0,0,1,0,1,0,0,1,0,0},
            {0,0,0,1,0,1,0,0,0,0},
            {0,1,0,0,1,0,1,0,0,0},
            {0,0,0,0,0,1,0,0,0,1},
            {0,0,0,1,0,0,0,0,1,0},
            {0,0,0,0,0,0,0,1,0,1},
            {0,0,0,0,0,0,1,0,1,0}};

        streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_12);
        streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_12);

        streit.SYMMETRISCH = RICHTIG;

		break;

    case 13:    // Science Information Network, (SINET)
        streit.DATEI_NAME = "SINET";

        streit.ANZAHL_KANTEN = 26;
        streit.ANZAHL_KNOTEN = 11;

        streit.GLEICHE_KAPAZITÄT = 236;

        int topology_13[11][11] =
            {{0,1,0,1,0,0,0,0,0,0,0},
            {1,0,1,0,0,0,0,0,0,0,0},
            {0,1,0,0,1,0,0,0,0,0,0},
            {1,0,0,0,1,1,0,0,0,0,0},
            {0,0,1,1,0,0,1,0,0,0,0},
            {0,0,0,1,0,0,0,1,1,0,0},
            {0,0,0,0,1,0,0,1,0,0,0},
            {0,0,0,0,0,1,1,0,0,1,0},
            {0,0,0,0,0,1,0,0,0,0,1},
            {0,0,0,0,0,0,0,1,0,0,1},
            {0,0,0,0,0,0,0,0,1,1,0}};

        streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_13);
        streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_13);

        streit.SYMMETRISCH = RICHTIG;

		break;

    case 14:    // CompuServe
		streit.DATEI_NAME = "CompuServe";

		streit.ANZAHL_KANTEN = 28;
		streit.ANZAHL_KNOTEN = 11;

		streit.GLEICHE_KAPAZITÄT = 45;

		int topology_14[11][11] =
			{{0,1,0,0,1,0,0,0,0,0,0},
			 {1,0,1,0,0,0,0,0,0,0,0},
			 {0,1,0,1,0,1,0,0,0,0,0},
			 {0,0,1,0,0,0,0,1,0,0,0},
			 {1,0,0,0,0,1,0,0,1,0,0},
			 {0,0,1,0,1,0,1,0,0,1,0},
			 {0,0,0,0,0,1,0,1,0,0,1},
			 {0,0,0,1,0,0,1,0,0,0,0},
			 {0,0,0,0,1,0,0,0,0,1,0},
			 {0,0,0,0,0,1,0,0,1,0,1},
			 {0,0,0,0,0,0,1,0,0,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_14);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_14);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 15:    // The Very-High Performance Backbone Network Service (vBNS)
		streit.DATEI_NAME = "vBNS";

		streit.ANZAHL_KANTEN = 34;
		streit.ANZAHL_KNOTEN = 12;

		streit.GLEICHE_KAPAZITÄT = 283;

		int topology_15[12][12] =
			{{0,1,0,1,0,0,0,0,0,0,0,0},
			 {1,0,1,1,0,0,0,0,0,0,0,0},
			 {0,1,0,1,1,1,0,0,0,0,0,0},
			 {1,1,1,0,0,0,1,0,0,0,0,0},
			 {0,0,1,0,0,1,0,0,0,0,0,0},
			 {0,0,1,0,1,0,0,1,0,0,0,0},
			 {0,0,0,1,0,0,0,1,1,0,0,0},
			 {0,0,0,0,0,1,1,0,0,1,0,0},
			 {0,0,0,0,0,0,1,0,0,0,1,1},
			 {0,0,0,0,0,0,0,1,0,0,0,1},
			 {0,0,0,0,0,0,0,0,1,0,0,1},
			 {0,0,0,0,0,0,0,0,1,1,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_15);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_15);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 16:    // Czech Education and Scientific Network (CESNET)

		streit.DATEI_NAME = "CESNET";

		streit.ANZAHL_KANTEN = 38;
		streit.ANZAHL_KNOTEN = 12;

		streit.GLEICHE_KAPAZITÄT = 317;

		int topology_16[12][12] =
			{{0,1,0,1,0,0,0,0,0,0,0,0},
			 {1,0,1,1,0,1,0,0,0,0,0,0},
			 {0,1,0,1,0,0,0,0,0,0,0,0},
			 {1,1,1,0,0,0,0,1,0,0,0,1},
			 {0,0,0,0,0,1,0,0,1,1,0,0},
			 {0,1,0,0,1,0,1,0,0,1,0,0},
			 {0,0,0,0,0,1,0,0,0,1,0,0},
			 {0,0,0,1,0,0,0,0,0,0,0,1},
			 {0,0,0,0,1,0,0,0,0,1,0,0},
			 {0,0,0,0,1,1,1,0,1,0,1,1},
			 {0,0,0,0,0,0,0,0,0,1,0,1},
			 {0,0,0,1,0,0,0,1,0,1,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_16);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_16);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 17:    // Australian Academic and Research Network (AARNET)
		streit.DATEI_NAME = "AARNET";

		streit.ANZAHL_KANTEN = 26;
		streit.ANZAHL_KNOTEN = 13;

		streit.GLEICHE_KAPAZITÄT = 200;

		int topology_17[13][13] =
			{{0,1,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,1,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,1,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,1,0,0,0,0,0,0,0},
			 {0,0,0,0,0,1,0,0,0,0,0,0,0},
			 {0,0,0,1,1,0,1,0,0,0,0,0,0},
			 {0,0,0,0,0,1,0,1,0,0,0,0,0},
			 {0,0,0,0,0,0,1,0,1,0,0,1,0},
			 {0,0,0,0,0,0,0,1,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,1,1,0},
			 {0,0,0,0,0,0,0,0,0,1,0,1,0},
			 {0,0,0,0,0,0,0,1,0,1,1,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_17);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_17);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 18:    // Italy Network (ITALY)
		streit.DATEI_NAME = "ITALY";

		streit.ANZAHL_KANTEN = 58;
		streit.ANZAHL_KNOTEN = 14;

		streit.GLEICHE_KAPAZITÄT = 414;

		int topology_18[14][14] =
			{{0,1,0,0,1,1,0,0,0,0,0,0,0,0},
			 {1,0,1,1,1,1,1,0,1,0,1,0,0,0},
			 {0,1,0,1,0,0,0,0,0,0,0,0,0,0},
			 {0,1,1,0,1,0,0,0,0,0,0,0,0,0},
			 {1,1,0,1,0,0,1,0,0,0,1,0,0,0},
			 {1,1,0,0,0,0,1,0,0,1,0,0,0,0},
			 {0,1,0,0,1,1,0,1,1,0,1,0,0,0},
			 {0,0,0,0,0,0,1,0,0,0,1,1,0,0},
			 {0,1,0,0,0,0,1,0,0,1,1,0,0,0},
			 {0,0,0,0,0,1,0,0,1,0,1,0,0,0},
			 {0,1,0,0,1,0,1,1,1,1,0,1,1,1},
			 {0,0,0,0,0,0,0,1,0,0,1,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,1,1,0,1},
			 {0,0,0,0,0,0,0,0,0,0,1,0,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_18);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_18);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 19:    // Austrian Academic Computer Network (ACONET)
		streit.DATEI_NAME = "ACONET";

		streit.ANZAHL_KANTEN = 44;
		streit.ANZAHL_KNOTEN = 15;

		streit.GLEICHE_KAPAZITÄT = 253;

		int topology_19[15][15] =
			{{0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,1,0,0,0,0,1,0,0,0,0,0,0,0},
			 {1,1,0,0,0,0,0,0,1,0,0,0,0,0,0},
			 {0,0,0,0,1,1,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,0,0,0,1,0,0,0,0,0,0},
			 {0,0,0,1,0,0,1,1,0,0,0,0,0,0,0},
			 {0,0,0,0,0,1,0,0,1,0,0,0,0,0,0},
			 {0,1,0,0,0,1,0,0,1,1,1,0,0,0,1},
			 {0,0,1,0,1,0,1,1,0,1,1,0,0,1,0},
			 {0,0,0,0,0,0,0,1,1,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,1,1,0,0,1,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,1,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,1,1,0,0,0},
			 {0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
			 {0,0,0,0,0,0,0,1,0,0,0,0,0,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_19);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_19);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 20:    // Mzima Backbone Network (MZIMA)
		streit.DATEI_NAME = "MZIMA";

		streit.ANZAHL_KANTEN = 36;
		streit.ANZAHL_KNOTEN = 14;

		streit.GLEICHE_KAPAZITÄT = 253;

		int topology_20[14][14] =
			{{0,1,1,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,1,0,1,0,0,0,0,0,0,0,0,0},
			 {1,1,0,1,0,0,1,0,0,0,0,0,0,0},
			 {0,0,1,0,1,0,0,0,0,0,0,0,0,0},
			 {0,1,0,1,0,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,1,0,0,0,1,0,0,0,0,0},
			 {0,0,1,0,0,0,0,1,0,1,0,0,0,0},
			 {0,0,0,0,0,0,1,0,1,0,0,0,0,0},
			 {0,0,0,0,0,1,0,1,0,0,1,0,0,1},
			 {0,0,0,0,0,0,1,0,0,0,0,0,1,0},
			 {0,0,0,0,0,0,0,0,1,0,0,1,0,1},
			 {0,0,0,0,0,0,0,0,0,0,1,0,1,0},
			 {0,0,0,0,0,0,0,0,0,1,0,1,0,0},
			 {0,0,0,0,0,0,0,0,1,0,1,0,0,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_20);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_20);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 21:    // Kiwi Advanced Research and Education Network (KAREN)
		streit.DATEI_NAME = "KAREN";

		streit.ANZAHL_KANTEN = 36;
		streit.ANZAHL_KNOTEN = 16;

		streit.GLEICHE_KAPAZITÄT = 213;

		int topology_21[16][16] =
			{{0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,1,1,0,1,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,0},
			 {0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
			 {0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_21);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_21);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 22:    // Belgian National Research and Education Network (BELNET)
		streit.DATEI_NAME = "BELNET";

		streit.ANZAHL_KANTEN = 40;
		streit.ANZAHL_KNOTEN = 17;

		streit.GLEICHE_KAPAZITÄT = 225;

		int topology_22[17][17] =
			{{0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,1,0,1,1,1,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0},
			 {0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0},
			 {0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0},
			 {0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0},
			 {0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0},
			 {0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_22);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_22);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 23:    // Indian Education and Research Network (ERNET)
		streit.DATEI_NAME = "ERNET";

		streit.ANZAHL_KANTEN = 36;
		streit.ANZAHL_KNOTEN = 16;

		streit.GLEICHE_KAPAZITÄT = 225;

		int topology_23[16][16] =
			{{0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
			 {0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
			 {0,1,0,0,0,0,0,0,1,0,0,1,1,1,0,0},
			 {0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0},
			 {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
			 {0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,1,0,0,0,0,0,0,1,0,0,0,0},
			 {0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0},
			 {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_23);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_23);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 24:    // Italian Academic and Research telecommunication network (GARR-B)
		streit.DATEI_NAME = "GARR-B";

		streit.ANZAHL_KANTEN = 50;
		streit.ANZAHL_KNOTEN = 16;

		streit.GLEICHE_KAPAZITÄT = 337;

		int topology_24[16][16] =
			{{0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0},
			 {0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0},
			 {0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,0,1,0,0,1,1,0,0,0,0},
			 {0,0,1,0,0,0,1,0,1,0,0,1,1,0,0,0},
			 {0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,0},
			 {0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,1},
			 {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_24);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_24);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 25:    // Slovenia Academic and Research Network (ARNES)
		streit.DATEI_NAME = "ARNES";

		streit.ANZAHL_KANTEN = 40;
		streit.ANZAHL_KNOTEN = 17;

		streit.GLEICHE_KAPAZITÄT = 235;

		int topology_25[17][17] =
			{{0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,0,1,1,1,1,0,0,0,0,0,0,0},
			 {0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,1,0,0,0,0,1,1,1,0,0,0,0},
			 {0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0},
			 {0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0},
			 {0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0},
			 {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1},
			 {0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_25);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_25);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 26:    // Germany Network (GERMANY)
		streit.DATEI_NAME = "GERMANY";

		streit.ANZAHL_KANTEN = 52;
		streit.ANZAHL_KNOTEN = 17;

		streit.GLEICHE_KAPAZITÄT = 306;

		int topology_26[17][17] =
			{{0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0},
			 {0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
			 {0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0},
			 {0,0,0,1,1,0,0,0,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,1,0,0,1,0,1,1,0,0,0,0,0,0},
			 {0,0,0,0,1,0,1,0,1,0,0,1,1,0,0,0,0},
			 {0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,1,1,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_26);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_26);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 27:    // RedIRIS (SPAIN)
		streit.DATEI_NAME = "SPAIN";

		streit.ANZAHL_KANTEN = 56;
		streit.ANZAHL_KNOTEN = 17;

		streit.GLEICHE_KAPAZITÄT = 329;

		int topology_27[17][17] =
			{{0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,1,0,1,1,1,1,1,1,1,1,0,0,0,0},
			 {0,1,1,0,1,0,0,0,0,0,0,0,0,1,0,0,0},
			 {0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0},
			 {0,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0},
			 {0,0,0,0,1,0,0,1,0,0,1,0,0,0,1,1,0},
			 {0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0},
			 {0,0,0,0,1,0,0,0,1,1,0,1,0,0,0,1,1},
			 {0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0},
			 {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
			 {0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_27);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_27);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 28:    // California Research and Education Network (CALREN)
		streit.DATEI_NAME = "CALREN";

		streit.ANZAHL_KANTEN = 38;
		streit.ANZAHL_KNOTEN = 16;

		streit.GLEICHE_KAPAZITÄT = 232;

		int topology_28[16][16] =
			{{0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0},
			 {0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0},
			 {0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,1,0,1,1,1,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,1,1,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,1},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
			 {0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_28);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_28);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 29:
		streit.DATEI_NAME = "LambdaRail";

		streit.ANZAHL_KANTEN = 44;
		streit.ANZAHL_KNOTEN = 18;

		streit.GLEICHE_KAPAZITÄT = 242;

		int topology_29[18][18] =
			{{0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,1,0,0,0,0,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0},
			 {0,0,0,0,0,0,1,1,0,0,0,0,1,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,0,0,0},
			 {0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_29);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_29);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 30:    // Memorex Network (MEMOREX)
		streit.DATEI_NAME = "MEMOREX";

		streit.ANZAHL_KANTEN = 48;
		streit.ANZAHL_KNOTEN = 19;

		streit.GLEICHE_KAPAZITÄT = 253;

		int topology_30[19][19] =
			{{0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,0},
			 {0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0},
			 {0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_30);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_30);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 31:	// Canada's Advanced Metwork (CANARIE)
		streit.DATEI_NAME = "CANARIE";

		streit.ANZAHL_KANTEN = 44;
		streit.ANZAHL_KNOTEN = 17;

		streit.GLEICHE_KAPAZITÄT = 274;

		int topology_31[17][17] =
			{{0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,1,0,0,1,1,1,0,0,0,0,0,0},
			 {0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0},
			 {0,0,0,0,1,0,1,0,0,0,1,1,0,0,0,0,0},
			 {0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_31);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_31);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 32:	// European Optical Network (EON)
		streit.DATEI_NAME = "EON";

		streit.ANZAHL_KANTEN = 74;
		streit.ANZAHL_KNOTEN = 19;

		streit.GLEICHE_KAPAZITÄT = 389;

		int topology_32[19][19] =
			{{0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
			 {1,1,0,0,0,0,1,0,1,1,1,0,0,0,0,0,0,0,0},
			 {0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0},
			 {0,1,0,0,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0},
			 {0,1,0,0,1,0,0,0,0,0,1,0,1,1,1,1,0,0,0},
			 {0,1,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,0},
			 {0,0,1,0,0,1,0,0,0,1,0,0,0,1,0,0,1,1,0},
			 {0,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
			 {0,0,0,1,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0},
			 {0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,1,0,0,0},
			 {0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0},
			 {0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,1,1},
			 {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_32);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_32);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 33:
		streit.DATEI_NAME = "ARPANET";

		streit.ANZAHL_KANTEN = 64;
		streit.ANZAHL_KNOTEN = 20;

		streit.GLEICHE_KAPAZITÄT = 320;

		int topology_33[20][20] =
			{{0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0},
			 {0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0},
			 {0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0},
			 {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0},
			 {0,0,0,0,0,1,0,0,0,0,0,1,1,0,0,1,0,0,0,0},
			 {0,0,0,0,0,1,0,0,0,0,1,0,0,1,0,0,0,0,1,0},
			 {0,0,0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0},
			 {0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,1},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_33);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_33);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 34:	// optosunet (SWEDEN)
		streit.DATEI_NAME = "SWEDEN";

		streit.ANZAHL_KANTEN = 48;
		streit.ANZAHL_KNOTEN = 20;

		streit.GLEICHE_KAPAZITÄT = 240;

		int topology_34[20][20] =
			{{0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,1,1,0,0,0,0,1,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,1,1,0,0,0,0,1,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_34);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_34);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 35:
		streit.DATEI_NAME = "HiberniaUSA";

		streit.ANZAHL_KANTEN = 54;
		streit.ANZAHL_KNOTEN = 20;

		streit.GLEICHE_KAPAZITÄT = 270;

		int topology_35[20][20] =
			{{0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0},
			 {0,0,0,0,1,0,0,0,1,0,0,1,0,1,0,0,0,0,0,0},
			 {0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
			 {0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,0,0,0,0},
			 {0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,1,0,0,0},
			 {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0},
			 {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_35);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_35);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 36:
		streit.DATEI_NAME = "PIONIE";

		streit.ANZAHL_KANTEN = 48;
		streit.ANZAHL_KNOTEN = 21;

		streit.GLEICHE_KAPAZITÄT = 238;

		int topology_36[21][21] =
			{{0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
			 {0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
			 {0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0},
			 {0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_36);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_36);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 37:	// Slovak Academic Network (SANET)
		streit.DATEI_NAME = "SANET";

		streit.ANZAHL_KANTEN = 54;
		streit.ANZAHL_KNOTEN = 25;

		streit.GLEICHE_KAPAZITÄT = 224;

		int topology_37[25][25] =
			{{0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,1,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_37);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_37);

		streit.SYMMETRISCH = RICHTIG;

		break;
	
	case 38:	// Finnish University and Research Network (FUNET)
		streit.DATEI_NAME = "FUNET";

		streit.ANZAHL_KANTEN = 54;
		streit.ANZAHL_KNOTEN = 24;

		streit.GLEICHE_KAPAZITÄT = 208;

		int topology_38[24][24] =
			{{0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,1,1,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_38);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_38);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 39:
		streit.DATEI_NAME = "NEWNET";

		streit.ANZAHL_KANTEN = 62;
		streit.ANZAHL_KNOTEN = 26;

		streit.GLEICHE_KAPAZITÄT = 238;

		int topology_39[26][26] =
			{{0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_39);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_39);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 40:	// Portugal Network (PORTUGAL)
		streit.DATEI_NAME = "PORTUGAL";

		streit.ANZAHL_KANTEN = 70;
		streit.ANZAHL_KNOTEN = 25;

		streit.GLEICHE_KAPAZITÄT = 277;

		int topology_40[25][25] =
			{{0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,1,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,1,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_40);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_40);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 41:	// National Telecommunication Network for Technology, Education and Research (RENATER)
		streit.DATEI_NAME = "RENATER";

		streit.ANZAHL_KANTEN = 70;
		streit.ANZAHL_KNOTEN = 27;

		streit.GLEICHE_KAPAZITÄT = 259;

		int topology_41[27][27] =
			{{0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,1,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
			 {0,0,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0},
			 {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
			 {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},
			 {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0},
			 {0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
			 {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_41);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_41);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 42:	// Louisiana Optical Network Initiative (LONI)
		streit.DATEI_NAME = "LONI";

		streit.ANZAHL_KANTEN = 74;
		streit.ANZAHL_KNOTEN = 33;

		streit.GLEICHE_KAPAZITÄT = 224;

		int topology_42[33][33] =
			{{0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,1,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_42);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_42);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 43:	// Metrona Network (METRONA)
		streit.DATEI_NAME = "METRONA";

		streit.ANZAHL_KANTEN = 82;
		streit.ANZAHL_KNOTEN = 32;

		streit.GLEICHE_KAPAZITÄT = 248;

		int topology_43[32][32] =
			{{0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,1,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_43);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_43);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 44:	// Global Communication Net AD (BULGARIA)
		streit.DATEI_NAME = "BULGARIA";

		streit.ANZAHL_KANTEN = 66;
		streit.ANZAHL_KNOTEN = 32;

		streit.GLEICHE_KAPAZITÄT = 206;

		int topology_44[32][32] =
			{{0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},
			 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_44);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_44);

		streit.SYMMETRISCH = RICHTIG;

		break;

	case 45:	// Minimal topology(3 Tenants)
		streit.DATEI_NAME = "MINTOPO3ENANTS";

		streit.ANZAHL_KANTEN = 8;
		streit.ANZAHL_KNOTEN = 5;

		streit.GLEICHE_KAPAZITÄT = 360;

		int topology_45[5][5] =
			{{0,0,0,1,0},
			 {0,0,0,1,0},
			 {0,0,0,1,0},
			 {1,1,1,0,1},
			 {0,0,0,1,0}};

		streit.BOGEN = bereichkopieren(streit.ANZAHL_KNOTEN, topology_45);
		streit.LISTE = bereichkopieren(streit.ANZAHL_KNOTEN, topology_45);

		streit.SYMMETRISCH = RICHTIG;

		break;
    }

	streit.GRENZE_VERKHERSEICHWITE = initialisierenGrenzeVerkherseichwite(streit.ANZAHL_KNOTEN);
	streit.UNTERSCHIEDLICHE_KAPAZITÄT = initialisierenUnterschiedliche_kapazität(streit.ANZAHL_KANTEN, streit.GLEICHE_KAPAZITÄT);

    return streit;
}

int zuordnungCode(char* name){
	if(strcmp(name, "OR") == ERFOLG)
		return 0;

    if(strcmp(name, "erste_modul") == ERFOLG)  // Hausaufgaben Vier
        return 1;

	if(strcmp(name, "NSFNET") == ERFOLG)
		return 2;
		
	if(strcmp(name, "USA-26") == ERFOLG)
		return 3;

	if(strcmp(name, "USA-28") == ERFOLG)
		return 4;

	if(strcmp(name, "11NODE_COST239") == ERFOLG)
		return 5;

	if(strcmp(name, "Pan-European") == ERFOLG)
		return 6;

    if(strcmp(name, "SANReN") == ERFOLG)    // South African National Research Network (SANReN)
	    return 7;

    if(strcmp(name, "ViaNetwork") == ERFOLG)
        return 8;

    if(strcmp(name, "BREN") == ERFOLG)
        return 9;

    if(strcmp(name, "RNP") == ERFOLG)
	    return 10;
    
    if(strcmp(name, "AbileneCore") == ERFOLG)
	    return 11;

    if(strcmp(name, "LEARN") == ERFOLG)
	    return 12;

    if(strcmp(name, "SINET") == ERFOLG)
	    return 13;

    if(strcmp(name, "CompuServe") == ERFOLG)
		return 14;

	if(strcmp(name, "vBNS") == ERFOLG)
		return 15;

	if(strcmp(name, "CESNET") == ERFOLG)
		return 16;

	if(strcmp(name, "AARNET") == ERFOLG)
		return 17;

	if(strcmp(name, "ITALY") == ERFOLG)
		return 18;

	if(strcmp(name, "ACONET") == ERFOLG)
		return 19;

	if(strcmp(name, "MZIMA") == ERFOLG)
		return 20;

	if(strcmp(name, "KAREN") == ERFOLG)
		return 21;

	if(strcmp(name, "BELNET") == ERFOLG)
		return 22;

	if(strcmp(name, "ERNET") == ERFOLG)
		return 23;

	if(strcmp(name, "GARR-B") == ERFOLG)
		return 24;

	if(strcmp(name, "ARNES") == ERFOLG)
		return 25;

	if(strcmp(name, "GERMANY") == ERFOLG)
		return 26;

	if(strcmp(name, "SPAIN") == ERFOLG)
		return 27;

	if(strcmp(name, "CALREN") == ERFOLG)
		return 28;

	if(strcmp(name, "LambdaRail") == ERFOLG)
		return 29;

	if(strcmp(name, "MEMOREX") == ERFOLG)
		return 30;
    
	if(strcmp(name, "CANARIE") == ERFOLG)
		return 31;

	if(strcmp(name, "EON") == ERFOLG)
		return 32;

	if(strcmp(name, "ARPANET") == ERFOLG)
		return 33;
	
	if(strcmp(name, "SWEDEN") == ERFOLG)
		return 34;

	if(strcmp(name, "HiberniaUSA") == ERFOLG)
		return 35;

	if(strcmp(name, "PIONIE") == ERFOLG)
		return 36;

	if(strcmp(name, "SANET") == ERFOLG)
		return 37;

	if(strcmp(name, "FUNET") == ERFOLG)
		return 38;

	if(strcmp(name, "NEWNET") == ERFOLG)
		return 39;
	
	if(strcmp(name, "PORTUGAL") == ERFOLG)
		return 40;

	if(strcmp(name, "RENATER") == ERFOLG)
		return 41;

	if(strcmp(name, "LONI") == ERFOLG)
		return 42;

	if(strcmp(name, "METRONA") == ERFOLG)
		return 43;

	if(strcmp(name, "BULGARIA") == ERFOLG)
		return 44;

	if(strcmp(name, "MINTOPO3ENANTS") == ERFOLG)
		return 45;

    return FEHLGESCHLAGEN;
}

int istbetriebsbereit(int code){
	if(code == 4)
		return FEHLGESCHLAGEN;

	return ERFOLG;
}

void eingebenUnterschiedliche_kapazität(struct Streit *streit){
	// Reservieren
}

// Externer Anruf : Über die Grenze der Verkhersreichwite

void erhaltenGrenzeVerkhersreichwite_INDEX_BEGRENZT(struct Streit *streit){
	for(int quelle_index=0;quelle_index<streit->ANZAHL_KNOTEN;quelle_index++)
		for(int ziel_index=0;ziel_index<streit->ANZAHL_KNOTEN;ziel_index++)
			if(quelle_index!=ziel_index){
				int minimale_index = MIN_INDEX(quelle_index, ziel_index);
				int maximale_index = MAX_INDEX(quelle_index, ziel_index);

				streit->GRENZE_VERKHERSEICHWITE[0][quelle_index][ziel_index] = minimale_index;
				streit->GRENZE_VERKHERSEICHWITE[1][quelle_index][ziel_index] = maximale_index;
			}
}

void erhaltenGrenzeVerkhersreichwite_ZUFÄLLIGE_MISCHUNG(struct Streit *streit){
	int Seed_Wert = zuordnungCode(streit->DATEI_NAME);
    srand(*(time_t *)&Seed_Wert);

    for(int quelle_index=0;quelle_index<streit->ANZAHL_KNOTEN;quelle_index++)
		for(int ziel_index=0;ziel_index<streit->ANZAHL_KNOTEN;ziel_index++)
			if(quelle_index!=ziel_index){
				int minimale_index = MIN_INDEX(quelle_index, ziel_index);
				int maximale_index = MAX_INDEX(quelle_index, ziel_index);

#ifdef ALLE_GLEICHE_KAPAZITÄT
				double median = (double)streit->GLEICHE_KAPAZITÄT/(2*(streit->ANZAHL_KNOTEN)*(streit->ANZAHL_KNOTEN));
				double schrittlänge = (double)streit->GLEICHE_KAPAZITÄT/(2*(streit->ANZAHL_KNOTEN)*(streit->ANZAHL_KNOTEN)*(streit->ANZAHL_KNOTEN));
   
#else
				double median = (double)berechnenMedianUnterschiedliche_kapazität(streit) /(2*(streit->ANZAHL_KNOTEN)*(streit->ANZAHL_KNOTEN));
				double schrittlänge = (double)streit->berechnenMedianUnterschiedliche_kapazität(streit)/(2*(streit->ANZAHL_KNOTEN)*(streit->ANZAHL_KNOTEN)*(streit->ANZAHL_KNOTEN));
#endif
				streit->GRENZE_VERKHERSEICHWITE[0][quelle_index][ziel_index] = median - (double)((1 - MINIMALE_ZUFÄLLIGER_BEREICH)*rand()/(RAND_MAX + 1.0) + MINIMALE_ZUFÄLLIGER_BEREICH)*schrittlänge*minimale_index;
				streit->GRENZE_VERKHERSEICHWITE[1][quelle_index][ziel_index] = median + (double)((MAXIMALE_ZUFÄLLIGER_BEREICH - MINIMALE_ZUFÄLLIGER_BEREICH)*rand()/(RAND_MAX + 1.0) + MINIMALE_ZUFÄLLIGER_BEREICH)*schrittlänge*maximale_index;               
			}
}

void erhaltenGrenzeVerkhersreichwite_STABILWERT(struct Streit *streit, double untere_grenze, double obere_grenze){
	for(int quelle_index=0;quelle_index<streit->ANZAHL_KNOTEN;quelle_index++)
		for(int ziel_index=0;ziel_index<streit->ANZAHL_KNOTEN;ziel_index++)
			if(quelle_index!=ziel_index){
				streit->GRENZE_VERKHERSEICHWITE[0][quelle_index][ziel_index] = untere_grenze;
				streit->GRENZE_VERKHERSEICHWITE[1][quelle_index][ziel_index] = obere_grenze;
			}
}

void erhaltenGrenzeVerkhersreichwite_STABILGRENZE(struct Streit *streit, double*** grenze){
	for(int quelle_index=0;quelle_index<streit->ANZAHL_KNOTEN;quelle_index++)
		for(int ziel_index=0;ziel_index<streit->ANZAHL_KNOTEN;ziel_index++)
			if(quelle_index!=ziel_index){
				streit->GRENZE_VERKHERSEICHWITE[0][quelle_index][ziel_index] = grenze[0][quelle_index][ziel_index];
				streit->GRENZE_VERKHERSEICHWITE[1][quelle_index][ziel_index] = grenze[1][quelle_index][ziel_index];
			}
}

// Externer Anruf : Suchen Sie den Header- oder Tail-Knoten

int suchenHeaderKnoten(struct Streit streit, int kant_index){
	int ANZAHL_KNOTEN = streit.ANZAHL_KNOTEN;
    int** LISTE = streit.LISTE;
    
    int verknupfüng = 1;
	for(int knote_index_i=0;knote_index_i<=(ANZAHL_KNOTEN-1);knote_index_i++)
		for(int knote_index_j = 0;knote_index_j<=(ANZAHL_KNOTEN-1);knote_index_j++)
			if(LISTE[knote_index_i][knote_index_j] == RICHTIG){
				LISTE[knote_index_i][knote_index_j] = verknupfüng;
				verknupfüng++;
			}

    for(int knote_index_i=0;knote_index_i<=(ANZAHL_KNOTEN-1);knote_index_i++)
		for(int knote_index_j = 0;knote_index_j<=(ANZAHL_KNOTEN-1);knote_index_j++)
            if(kant_index+1 == LISTE[knote_index_i][knote_index_j])
                return knote_index_i;
    return FEHLGESCHLAGEN;
}

int suchenSchwanzKnoten(struct Streit streit, int kant_index){
	int ANZAHL_KNOTEN = streit.ANZAHL_KNOTEN;
    int** LISTE = streit.LISTE;
    
    int verknupfüng = 1;
	for(int knote_index_i=0;knote_index_i<=(ANZAHL_KNOTEN-1);knote_index_i++)
		for(int knote_index_j = 0;knote_index_j<=(ANZAHL_KNOTEN-1);knote_index_j++)
			if(LISTE[knote_index_i][knote_index_j] == RICHTIG){
				LISTE[knote_index_i][knote_index_j] = verknupfüng;
				verknupfüng++;
			}

    for(int knote_index_i=0;knote_index_i<=(ANZAHL_KNOTEN-1);knote_index_i++)
		for(int knote_index_j = 0;knote_index_j<=(ANZAHL_KNOTEN-1);knote_index_j++)
            if(kant_index+1 == LISTE[knote_index_i][knote_index_j])
                return knote_index_j;
    return FEHLGESCHLAGEN;
}

// Externer Anruf : Drucken
void druckenTopology(struct Streit streit){
	printf("   ");
	for(int quelle_index=0;quelle_index<streit.ANZAHL_KNOTEN;quelle_index++)
		printf(" "DRUCKEN_INTEGER" ", quelle_index+1);
		printf("\n");

	for(int quelle_index=0;quelle_index<streit.ANZAHL_KNOTEN;quelle_index++){
		printf(DRUCKEN_INTEGER" ", quelle_index+1);
		for(int ziel_index=0;ziel_index<streit.ANZAHL_KNOTEN;ziel_index++)
			printf("  %d  ", streit.BOGEN[quelle_index][ziel_index]);
		printf("\n");
	}
}

void druckenTopologyZahl(struct Streit streit){
	int ANZAHL_KNOTEN = streit.ANZAHL_KNOTEN;
    int** LISTE = streit.LISTE;
    
    int verknupfüng = 1;
	for(int knote_index_i=0;knote_index_i<=(ANZAHL_KNOTEN-1);knote_index_i++)
		for(int knote_index_j = 0;knote_index_j<=(ANZAHL_KNOTEN-1);knote_index_j++)
			if(LISTE[knote_index_i][knote_index_j] == RICHTIG){
				LISTE[knote_index_i][knote_index_j] = verknupfüng;
				verknupfüng++;
			}

	printf("   ");
	for(int quelle_index=0;quelle_index<streit.ANZAHL_KNOTEN;quelle_index++)
		printf(" "DRUCKEN_INTEGER" ", quelle_index+1);
		printf("\n");

	for(int quelle_index=0;quelle_index<streit.ANZAHL_KNOTEN;quelle_index++){
		printf(DRUCKEN_INTEGER" ", quelle_index+1);
		for(int ziel_index=0;ziel_index<streit.ANZAHL_KNOTEN;ziel_index++)
			printf("  %d  ", LISTE[quelle_index][ziel_index]);
		printf("\n");
	}
}

void druckenGrenzeVerkhersreichwite(struct Streit streit){

	for(int quelle_index=0;quelle_index<streit.ANZAHL_KNOTEN;quelle_index++){
		for(int ziel_index=0;ziel_index<streit.ANZAHL_KNOTEN;ziel_index++)
			if(quelle_index!=ziel_index)
				printf(DRUCKEN_DOUBLE" <= d_%02d_%02d <= "DRUCKEN_DOUBLE"\n", streit.GRENZE_VERKHERSEICHWITE[0][quelle_index][ziel_index], quelle_index+1, ziel_index+1, streit.GRENZE_VERKHERSEICHWITE[1][quelle_index][ziel_index]);
		printf("\n");
	}
}

void druckenInformation(struct Streit streit){
	printf("Topology name          : %s\n", streit.DATEI_NAME);
	printf("\nNumber of links        : "DRUCKEN_INTEGER"\n", streit.ANZAHL_KANTEN);
	printf("Number of nodes        : "DRUCKEN_INTEGER"\n\n", streit.ANZAHL_KANTEN);
	
	
#ifdef ALLE_GLEICHE_KAPAZITÄT
	printf("Link capacity          : "DRUCKEN_DOUBLE"\n", streit.ANZAHL_KANTEN);
#else
	for(int kant_index=0;kant_index<ANZAHL_KANTEN;kant_index++)
		printf("Link %2d with capacity : "DRUCKEN_DOUBLE"\n", *(streit.UNTERSCHIEDLICHE_KAPAZITÄT+kant_index));
#endif
}

void druckenKantenmitMatrix(struct Streit streit){
	int ANZAHL_KNOTEN = streit.ANZAHL_KNOTEN;
    int** LISTE = streit.LISTE;
    
    int verknupfüng = 1;
	for(int knote_index_i=0;knote_index_i<=(ANZAHL_KNOTEN-1);knote_index_i++)
		for(int knote_index_j = 0;knote_index_j<=(ANZAHL_KNOTEN-1);knote_index_j++)
			if(LISTE[knote_index_i][knote_index_j] == RICHTIG){
				LISTE[knote_index_i][knote_index_j] = verknupfüng;
				verknupfüng++;
			}

	printf("     ");
	for(int knote_index_i=0;knote_index_i<=(ANZAHL_KNOTEN-1);knote_index_i++)
		printf(" "DRUCKEN_INTEGER" ", knote_index_i);
	printf("\n");

	for(int knote_index_i=0;knote_index_i<=(ANZAHL_KNOTEN-1);knote_index_i++){
		printf(" "DRUCKEN_INTEGER" ", knote_index_i);
		for(int knote_index_j = 0;knote_index_j<=(ANZAHL_KNOTEN-1);knote_index_j++)
			printf(" "DRUCKEN_INTEGER" ", LISTE[knote_index_i][knote_index_j]);
		printf("\n");
	}
}

void druckenKantKnoten(struct Streit streit, int kant_index){
	printf("Kant "DRUCKEN_INTEGER" = ("DRUCKEN_INTEGER", "DRUCKEN_INTEGER")\n", kant_index+1, suchenHeaderKnoten(streit, kant_index), suchenSchwanzKnoten(streit, kant_index));
}

# endif