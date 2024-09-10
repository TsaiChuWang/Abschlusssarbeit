#include <gsl/gsl_cdf.h>    // sudo apt-get install libgsl-dev
#define GAUSSIAN

#ifdef GAUSSIAN

/**
 * @brief Prints the matrix of mu(mean) values for each source and destination node.
 * 
 * This function prints the matrix `mu_bedeutung`, which represents some values associated
 * with the nodes in the graph or network defined by the `Streit` structure. Each value in the 
 * matrix corresponds to the relationship or significance between a source node (`quelle_index`) 
 * and a destination node (`ziel_index`).
 * 
 * @param streit A structure of type `Streit` containing network information, including the 
 * number of nodes (`ANZAHL_KNOTEN`).
 * @param mu_bedeutung A double pointer representing a 2D array (matrix) of size 
 * [ANZAHL_KNOTEN][ANZAHL_KNOTEN], which holds the significance or importance values 
 * between nodes.
 */
void druckenMuBedeutung(struct Streit streit, double** mu_bedeutung){
    int ANZAHL_KNOTEN = streit.ANZAHL_KNOTEN;
    for(int quelle_index=0;quelle_index<ANZAHL_KNOTEN;quelle_index++){
        for(int ziel_index=0;ziel_index<ANZAHL_KNOTEN;ziel_index++)
            printf(" "DRUCKEN_DOUBLE" ", mu_bedeutung[quelle_index][ziel_index]);
        printf("\n");
    }
}

/**
 * @brief Prints the matrix of sigma(convariance) values for each source and destination node.
 * 
 * This function prints the matrix `sigma_kovarianz`, which represents some values associated
 * with the nodes in the graph or network defined by the `Streit` structure. Each value in the 
 * matrix corresponds to the relationship or significance between a source node (`quelle_index`) 
 * and a destination node (`ziel_index`).
 * 
 * @param streit A structure of type `Streit` containing network information, including the 
 * number of nodes (`ANZAHL_KNOTEN`).
 * @param mu_bedeutung A double pointer representing a 2D array (matrix) of size 
 * [ANZAHL_KNOTEN][ANZAHL_KNOTEN], which holds the significance or importance values 
 * between nodes.
 */
void druckenSigmaKovarianz(struct Streit streit, double** sigma_kovarianz){
    int ANZAHL_KNOTEN = streit.ANZAHL_KNOTEN;
    for(int quelle_index=0;quelle_index<ANZAHL_KNOTEN;quelle_index++){
        for(int ziel_index=0;ziel_index<ANZAHL_KNOTEN;ziel_index++)
            printf(" "DRUCKEN_DOUBLE" ", sigma_kovarianz[quelle_index][ziel_index]);
        printf("\n");
    }
}

/**
 * @brief Allocates and calculates the mean significance matrix (mu_bedeutung) for Gaussian-distributed traffic demands.
 * 
 * This function initializes and calculates a matrix (`mu_bedeutung`) that stores the mean values of Gaussian-distributed 
 * random variables representing traffic demands (`d_{st}`) between nodes in a network. The mean is computed as the 
 * average of the traffic demand interval (`GRENZE_VERKHERSEICHWITE`) provided in the `Streit` structure.
 * 
 * The interval is assumed to represent a boundary for the random variables, and the mean is calculated for each 
 * origin-destination pair. If the source and destination nodes are the same, the value in the matrix is set to 0.
 * 
 * @param streit A structure of type `Streit` that contains:
 *   - The number of nodes (`ANZAHL_KNOTEN`)
 *   - The traffic demand interval (`GRENZE_VERKHERSEICHWITE`) for each node pair
 *   - A file name (`DATEI_NAME`) for reference or debugging purposes.
 * 
 * @return A double pointer representing a 2D array (matrix) of size [ANZAHL_KNOTEN][ANZAHL_KNOTEN],
 * which holds the mean values of the Gaussian random variables between nodes.
 * 
 * The matrix is dynamically allocated using `malloc` and will need to be freed later.
 */
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

/**
 * @brief Allocates and calculates the covariance significance matrix (sigma_kovarianz) for Gaussian-distributed traffic demands.
 * 
 * This function initializes and calculates a matrix (`sigma_kovarianz`) that stores the covariance values of Gaussian-distributed 
 * random variables representing traffic demands (`d_{st}`) between nodes in a network. The covariance is computed as the 
 * average of the traffic demand interval (`GRENZE_VERKHERSEICHWITE`) provided in the `Streit` structure.
 * 
 * The interval is assumed to represent a boundary for the random variables, and the covariance is calculated for each 
 * origin-destination pair. If the source and destination nodes are the same, the value in the matrix is set to 0.
 * 
 * @param streit A structure of type `Streit` that contains:
 *   - The number of nodes (`ANZAHL_KNOTEN`)
 *   - The traffic demand interval (`GRENZE_VERKHERSEICHWITE`) for each node pair
 *   - A file name (`DATEI_NAME`) for reference or debugging purposes.
 * 
 * @return A double pointer representing a 2D array (matrix) of size [ANZAHL_KNOTEN][ANZAHL_KNOTEN],
 * which holds the covariance values of the Gaussian random variables between nodes.
 * 
 * The matrix is dynamically allocated using `malloc` and will need to be freed later.
 */
double** erhaltenKovarianz(struct Streit streit){
    int ANZAHL_KNOTEN = streit.ANZAHL_KNOTEN;
    double** sigma_kovarianz = (double**)malloc(sizeof(double*)*ANZAHL_KNOTEN);
    for(int index=0;index<ANZAHL_KNOTEN;index++)
        *(sigma_kovarianz+index) = (double*)malloc(sizeof(double)*ANZAHL_KNOTEN);

    for(int quelle_index=0;quelle_index<ANZAHL_KNOTEN;quelle_index++)
        for(int ziel_index=0;ziel_index<ANZAHL_KNOTEN;ziel_index++)
            if(quelle_index==ziel_index)
                sigma_kovarianz[quelle_index][ziel_index] = 0.0;
            else sigma_kovarianz[quelle_index][ziel_index] = (streit.GRENZE_VERKHERSEICHWITE[1][quelle_index][ziel_index]-streit.GRENZE_VERKHERSEICHWITE[0][quelle_index][ziel_index])/(2.0);

#ifdef DRUCKEN_SIGMA_KOVARIANZ_IM_ERHALTEN
    printf("σ of %s :\n", streit.DATEI_NAME);
    druckenMuBedeutung(streit, sigma_kovarianz);
#endif

    return sigma_kovarianz;           
}

#endif