#include <gsl/gsl_cdf.h>    // sudo apt-get install libgsl-dev
#define GAUSSIAN

#ifdef GAUSSIAN

void druckenMuBedeutung(struct Streit streit, double** mu_bedeutung){
    int ANZAHL_KNOTEN = streit.ANZAHL_KNOTEN;
    for(int quelle_index=0;quelle_index<ANZAHL_KNOTEN;quelle_index++){
        for(int ziel_index=0;ziel_index<ANZAHL_KNOTEN;ziel_index++)
            printf(" "DRUCKEN_DOUBLE" ", mu_bedeutung[quelle_index][ziel_index]);
        printf("\n");
    }
}

double** erhaltenBedeutung(struct Streit streit){
    int ANZAHL_KNOTEN = streit.ANZAHL_KNOTEN;
    double** mu_bedeutung = (double**)malloc(sizeof(double*)*ANZAHL_KNOTEN);
    for(int index=0;index<ANZAHL_KNOTEN;index++)
        *(mu_bedeutung+index) = (double*)malloc(sizeof(double)*ANZAHL_KNOTEN);

    for(int quelle_index=0;quelle_index<ANZAHL_KNOTEN;quelle_index++)
        for(int ziel_index=0;ziel_index<ANZAHL_KNOTEN;ziel_index++)
            if(quelle_index==ziel_index)
                mu_bedeutung[quelle_index][ziel_index] = 0.0;
            else mu_bedeutung[quelle_index][ziel_index] = (streit.GRENZE_VERKHERSEICHWITE[1][quelle_index][ziel_index]+streit.GRENZE_VERKHERSEICHWITE[0][quelle_index][ziel_index])/(2.0);

#ifdef DRUCKEN_MU_BEDEUTUNG_IM_ERHALTEN
    printf("μ of %s :\n", streit.DATEI_NAME);
    druckenMuBedeutung(streit, mu_bedeutung);
#endif

    return mu_bedeutung;           
}

#endif