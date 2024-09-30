
#include <curl/curl.h>	// sudo apt-get install libcurl4-openssl-dev

#include "../Enthalten/Konfiguration.h"
#include "../Enthalten/Aufbau.h"
#include "../Enthalten/Streit.h"
#include "../Enthalten/Netz.h"
#include "../Enthalten/Guassian.h"
#include "../Enthalten/Parser.h"

#define PYTHON_SKRIPT_WEG "../Quelle/Haupt_CVXPY.py"

#define SOLVER_CVXOPT   "CVXOPT"    // LP || SOCP || SDP        General-purpose convex optimization solver
#define SOLVER_SCS      "SCS"       // LP || SOCP || SDP || QP  Splitting Conic Solver
#define SOLVER_GLPK     "GLPK"      // LP || MILP               Solves linear programs (LP) and mixed-integer linear programs (MILP)
#define SOLVER_GLPK_MI  "GLPK_MI"   // LP || MILP               GLPK Mixed-Integer Solver
#define SOLVER_GUROBI   "GUROBI"    // LP || SOCP || MILP || QP Commercial solver

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

    int solver = atoi(argv[2]);
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
#ifdef NUR_TRANSFER_VERKEHR_BEOBACHTUNG
    druckenAusgehendKanten(streit);
#endif
#endif

    // Scalter
    int einschränkungszähler = 1;
    int schalter = 0;
    int quadratischschalter =1;

    // Konstante
    double** mu_bedeutung = erhaltenBedeutung(streit);
    double** sigma_kovarianz = erhaltenKovarianz(streit);
#ifdef NUR_TRANSFER_VERKEHR_BEOBACHTUNG
    for(int index=0;index<ANZAHL_KNOTEN;index++)
        for(int jndex=0;jndex<ANZAHL_KNOTEN;jndex++)
            if(istInODPaar(index,jndex,streit)==FALSCH){
                mu_bedeutung[index][jndex] = 0.0;
                sigma_kovarianz[index][jndex] = 0.0;
            }
#endif

    double umgekehrt_Gaussian = gsl_cdf_gaussian_Pinv(ETA, 1.0);
    int OD_paar_zahl;

    // Öffnen Sie die Textdatei
	FILE *dateizeiger;
    dateizeiger = fopen(PYTHON_SKRIPT_WEG,"w+");

    // Import library
    fprintf(dateizeiger, "import cvxpy\n");
    fprintf(dateizeiger, "\n");

    // Constant definition
    fprintf(dateizeiger, "nodes = %d\n", streit.ANZAHL_KNOTEN);
    fprintf(dateizeiger, "edges = %d\n", streit.ANZAHL_KANTEN);
#ifdef NUR_TRANSFER_VERKEHR_BEOBACHTUNG
    fprintf(dateizeiger, "pairs = %d\n", streit.OD_paar_zahl);
    OD_paar_zahl = streit.OD_paar_zahl;
#else
    fprintf(dateizeiger, "pairs = %d\n", streit.ANZAHL_KNOTEN*(streit.ANZAHL_KNOTEN-1));
    OD_paar_zahl = streit.ANZAHL_KNOTEN*(streit.ANZAHL_KNOTEN-1);
#endif
#ifdef ALLE_GLEICHE_KAPAZITÄT
    fprintf(dateizeiger, "capacity = %f\n", streit.GLEICHE_KAPAZITÄT);
#else
    printf(KRED_L"FEHLER: FEHLT DIESES MODELL IST NOCH NICHT VOLLSTÄNDIG!\n"RESET);
	return FEHLGESCHLAGEN;
#endif
    fprintf(dateizeiger, "error = %f\n", 1-ETA);
    fprintf(dateizeiger, "mean_mu = [");
    for(int index=0;index<OD_paar_zahl;index++)
        if(index == OD_paar_zahl-1)
            fprintf(dateizeiger, "%f ]\n", mu_bedeutung[streit.OD_paar[index][0]][streit.OD_paar[index][1]]);
        else fprintf(dateizeiger, "%f, ", mu_bedeutung[streit.OD_paar[index][0]][streit.OD_paar[index][1]]);
    fprintf(dateizeiger, "standard_derivation_sigma = [");
    for(int index=0;index<OD_paar_zahl;index++)
        if(index == OD_paar_zahl-1)
            fprintf(dateizeiger, "%f ]\n", sigma_kovarianz[streit.OD_paar[index][0]][streit.OD_paar[index][1]]);
        else fprintf(dateizeiger, "%f, ", sigma_kovarianz[streit.OD_paar[index][0]][streit.OD_paar[index][1]]);

    fprintf(dateizeiger, "\n");

    // Variable definition
    fprintf(dateizeiger, "fst = cvxpy.Variable((pairs ,edges), nonneg=True)\n");
    fprintf(dateizeiger, "alpha = cvxpy.Variable(nonneg=True)\n");
    fprintf(dateizeiger, "\n");

    // Constraints
    fprintf(dateizeiger, "objective = cvxpy.Minimize(alpha)\n");
    fprintf(dateizeiger, "constraints = [\n");
    // Probability constraint
    for(int kant_index=0;kant_index<streit.ANZAHL_KANTEN;kant_index++){
        fprintf(dateizeiger, "\t"DRUCKEN_DOUBLE"*cvxpy.norm(", umgekehrt_Gaussian);
        for(int index=0;index<OD_paar_zahl;index++){
            double coeffizient = sigma_kovarianz[streit.OD_paar[index][0]][streit.OD_paar[index][1]]/mu_bedeutung[streit.OD_paar[index][0]][streit.OD_paar[index][1]];
            if(sigma_kovarianz[streit.OD_paar[index][0]][streit.OD_paar[index][1]]*mu_bedeutung[streit.OD_paar[index][0]][streit.OD_paar[index][1]] == 0)
                coeffizient = 0;
            fprintf(dateizeiger, DRUCKEN_DOUBLE"*fst[%d][%d]", coeffizient, index, kant_index);
            if(index==OD_paar_zahl-1)
                fprintf(dateizeiger, ", 2) <= capacity*alpha - (");
            else fprintf(dateizeiger, " + ");
        }
        for(int index=0;index<OD_paar_zahl;index++){
            fprintf(dateizeiger, "fst[%d][%d]", index, kant_index);
            if(index==OD_paar_zahl-1)
                fprintf(dateizeiger, "),\n");
            else fprintf(dateizeiger, " + ");
        }
    }
    fprintf(dateizeiger, "\n");

    // Flow conservation
    schalter = 0;
    for(int knote_index=0;knote_index<=ANZAHL_KNOTEN;knote_index++){
        for(int index=0;index<OD_paar_zahl;index++){
            fprintf(dateizeiger, "\t");
            for(int kant_index=0;kant_index<streit.ANZAHL_KANTEN;kant_index++){
                if(suchenHeaderKnoten(streit,kant_index)==knote_index){
                    if(schalter == 0)
                        schalter = 1;
                    else fprintf(dateizeiger, " + ");
                    fprintf(dateizeiger, "fst[%d][%d]", index, kant_index);
                }

                if(suchenSchwanzKnoten(streit,kant_index)==knote_index){
                    if(schalter == 0)
                        schalter = 1;

                    fprintf(dateizeiger, " - fst[%d][%d]", index, kant_index);
                }
            }

            if(knote_index==streit.OD_paar[index][0])
                fprintf(dateizeiger, " == "DRUCKEN_DOUBLE, mu_bedeutung[streit.OD_paar[index][0]][streit.OD_paar[index][1]]);
            else if(knote_index==streit.OD_paar[index][1])
                fprintf(dateizeiger, "== -"DRUCKEN_DOUBLE, mu_bedeutung[streit.OD_paar[index][0]][streit.OD_paar[index][1]]);
            else if(schalter!=0)
                fprintf(dateizeiger, "== 0");

            if(knote_index==ANZAHL_KNOTEN-1 && index==OD_paar_zahl-1)
                fprintf(dateizeiger, "\n");
            else if(schalter!=0)
                fprintf(dateizeiger, ",\n");
            schalter = 0;
        }
    }
    fprintf(dateizeiger, "]\n");
    fprintf(dateizeiger, "\n");

    fprintf(dateizeiger, "problem = cvxpy.Problem(objective, constraints)\n");
    switch(solver){
        case 0:
            fprintf(dateizeiger, "problem.solve(solver=cvxpy.%s)\n", SOLVER_CVXOPT);
            fprintf(dateizeiger, "print(\"optimal value with %s:\", problem.value)\n", SOLVER_CVXOPT);
        break;
        case 1:
            fprintf(dateizeiger, "problem.solve(solver=cvxpy.%s)\n", SOLVER_SCS);
            fprintf(dateizeiger, "print(\"optimal value with %s:\", problem.value)\n", SOLVER_SCS);
        break;
        case 2:
            fprintf(dateizeiger, "problem.solve(solver=cvxpy.%s)\n", SOLVER_GLPK);
            fprintf(dateizeiger, "print(\"optimal value with %s:\", problem.value)\n", SOLVER_GLPK);
        break;
        case 3:
            fprintf(dateizeiger, "problem.solve(solver=cvxpy.%s)\n", SOLVER_GLPK_MI);
            fprintf(dateizeiger, "print(\"optimal value with %s:\", problem.value)\n", SOLVER_GLPK_MI);
        break;
        case 4:
            fprintf(dateizeiger, "problem.solve(solver=cvxpy.%s)\n", SOLVER_GUROBI);
            fprintf(dateizeiger, "print(\"optimal value with %s:\", problem.value)\n", SOLVER_GUROBI);
        break;
        default:
            fprintf(dateizeiger, "problem.solve(solver=cvxpy.%s)\n", SOLVER_CVXOPT);
            fprintf(dateizeiger, "print(\"optimal value with %s:\", problem.value)\n", SOLVER_CVXOPT);
    }

    system("python3 "PYTHON_SKRIPT_WEG"\n");

    fprintf(dateizeiger, "\n");
    fclose(dateizeiger);

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