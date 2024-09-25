
#include <curl/curl.h>	// sudo apt-get install libcurl4-openssl-dev

#include "../Enthalten/Konfiguration.h"
#include "../Enthalten/Aufbau.h"
#include "../Enthalten/Streit.h"
#include "../Enthalten/Netz.h"
#include "../Enthalten/Guassian.h"
#include "../Enthalten/Parser.h"

#define MATLAB_SKRIPT_WEG "../Quelle/Haupt_SEDUMI.m"

// gcc ./Haupt_SEDUMI.c -o ../Ausführung/Haupt_SEDUMI -lcurl -lgsl -lgslcblas -lm
// ../Ausführung/Haupt_SEDUMI USA-26 
// ../Ausführung/Haupt_SEDUMI OR 

void printsigma(double** sigma_kovarianz, struct Streit streit){
    for(int index=0;index<streit.ANZAHL_KNOTEN;index++)
        for(int jndex=0;jndex<streit.ANZAHL_KNOTEN;jndex++)
            if(jndex==streit.ANZAHL_KNOTEN-1)
                printf(DRUCKEN_DOUBLE";\n", sigma_kovarianz[index][jndex]);
            else printf(DRUCKEN_DOUBLE", ", sigma_kovarianz[index][jndex]);
}

void printInputEdge(struct Streit streit, int knote_index){
    printf("\tcase %d\n\t\tA = [", knote_index+1);
    int start = 0;
    for(int kant_index=0;kant_index<streit.ANZAHL_KANTEN;kant_index++)
        if(suchenSchwanzKnoten(streit,kant_index)==knote_index)
            if(start==0){
                printf(" %d", kant_index+1);
                start = 1;
            }else printf(", %d", kant_index+1);
    printf("];\n");
}


void printOutputEdge(struct Streit streit, int knote_index){
    printf("\tcase %d\n\t\tB = ", knote_index+1);
    int start = 0;
    for(int kant_index=0;kant_index<streit.ANZAHL_KANTEN;kant_index++)
        if(suchenHeaderKnoten(streit,kant_index)==knote_index)
            if(start==0){
                printf(" %d", kant_index+1);
                start = 1;
            }else printf(", %d", kant_index+1);
    printf(";\n");
}

void printOutputEdgeArray(struct Streit streit, int knote_index){
    printf("\t");
    int start = 0;
    for(int kant_index=0;kant_index<streit.ANZAHL_KANTEN;kant_index++)
        if(suchenHeaderKnoten(streit,kant_index)==knote_index){
            if(start==0){
                printf(" 1");
                start = 1;
            }else printf(", 1");
        }else{
            if(start==0){
                printf(" 0");
                start = 1;
            }else printf(", 0");
        }
    printf(";\n");
}

void addPath(FILE* dateizeiger){
    fprintf(dateizeiger, "addpath('/home/wtsaichu/Dokumente/Installieren/YALMIP/YALMIP-develop')\n");
    fprintf(dateizeiger, "addpath('/home/wtsaichu/Dokumente/Installieren/YALMIP/YALMIP-develop/extras')\n");
    fprintf(dateizeiger, "addpath('/home/wtsaichu/Dokumente/Installieren/YALMIP/YALMIP-develop/solvers')\n");
    fprintf(dateizeiger, "addpath('/home/wtsaichu/Dokumente/Installieren/YALMIP/YALMIP-develop/modules')\n");
    fprintf(dateizeiger, "addpath('/home/wtsaichu/Dokumente/Installieren/YALMIP/YALMIP-develop/modules/parametric')\n");
    fprintf(dateizeiger, "addpath('/home/wtsaichu/Dokumente/Installieren/YALMIP/YALMIP-develop/modules/moment')\n");
    fprintf(dateizeiger, "addpath('/home/wtsaichu/Dokumente/Installieren/YALMIP/YALMIP-develop/modules/global')\n");
    fprintf(dateizeiger, "addpath('/home/wtsaichu/Dokumente/Installieren/YALMIP/YALMIP-develop/modules/robust')\n");
    fprintf(dateizeiger, "addpath('/home/wtsaichu/Dokumente/Installieren/YALMIP/YALMIP-develop/modules/sos')\n");
    fprintf(dateizeiger, "addpath('/home/wtsaichu/Dokumente/Installieren/YALMIP/YALMIP-develop/operators')\n");
    fprintf(dateizeiger, "addpath('/home/wtsaichu/Dokumente/Installieren/sedumi/')\n");
}

void defineVariable(FILE* dateizeiger, struct Streit streit){
    fprintf(dateizeiger, "E = %d\n", streit.ANZAHL_KANTEN);
    fprintf(dateizeiger, "N = %d\n\n", streit.ANZAHL_KNOTEN);

    fprintf(dateizeiger, "f_st = sdpvar(N, N, E); \n");
    fprintf(dateizeiger, "alpha = sdpvar(1); \n");
}

void defineConstant(FILE* dateizeiger, struct Streit streit){
    double umgekehrt_Gaussian = gsl_cdf_gaussian_Pinv(ETA, 1.0);
    fprintf(dateizeiger, "C1 = "DRUCKEN_DOUBLE";\n", umgekehrt_Gaussian);
    fprintf(dateizeiger, "C2 = "DRUCKEN_DOUBLE";\n", streit.GLEICHE_KAPAZITÄT);
}

void defineMu(FILE* dateizeiger, struct Streit streit, double** mu_bedeutung){
    fprintf(dateizeiger, "mu_st = [\n");
    for(int index=0;index<streit.ANZAHL_KNOTEN;index++)
        for(int jndex=0;jndex<streit.ANZAHL_KNOTEN;jndex++)
            if(jndex==streit.ANZAHL_KNOTEN-1)
                if(index==streit.ANZAHL_KNOTEN-1)
                    fprintf(dateizeiger, DRUCKEN_DOUBLE"];\n", mu_bedeutung[index][jndex]);
                else fprintf(dateizeiger, DRUCKEN_DOUBLE";\n", mu_bedeutung[index][jndex]);
            else fprintf(dateizeiger, DRUCKEN_DOUBLE", ", mu_bedeutung[index][jndex]);
}

void defineSigma(FILE* dateizeiger, struct Streit streit, double** sigma_kovarianz){
    // fprintf(dateizeiger, "for e = 1:E\n");
    fprintf(dateizeiger, "sigma_st = [\n");
    for(int index=0;index<streit.ANZAHL_KNOTEN;index++){
        // fprintf(dateizeiger, "\t\t");
        for(int jndex=0;jndex<streit.ANZAHL_KNOTEN;jndex++)
            if(jndex==streit.ANZAHL_KNOTEN-1)
                if(index==streit.ANZAHL_KNOTEN-1)
                    fprintf(dateizeiger, DRUCKEN_DOUBLE"];\n", sigma_kovarianz[index][jndex]);
                else fprintf(dateizeiger, DRUCKEN_DOUBLE";\n", sigma_kovarianz[index][jndex]);
            else fprintf(dateizeiger, DRUCKEN_DOUBLE", ", sigma_kovarianz[index][jndex]);
    }
    // fprintf(dateizeiger, "end\n");
}


void printInputEdgeRow(struct Streit streit, int knote_index){
    printf("\t");
    int start = 0;
    for(int kant_index=0;kant_index<streit.ANZAHL_KANTEN;kant_index++)
        if(suchenSchwanzKnoten(streit,kant_index)==knote_index){
            if(start==0){
                printf(" 1");
                start = 1;
            }else printf(", 1");
        }else{
            if(start==0){
                printf(" 0");
                start = 1;
            }else printf(", 0");
        }
    printf(";\n");
}

void flowConservationInput(FILE* dateizeiger, struct Streit streit){
    fprintf(dateizeiger, "input_c =[\n");
    for(int knote_index=0;knote_index<streit.ANZAHL_KNOTEN;knote_index++){
        fprintf(dateizeiger, "\t");
        int start = 0;
        for(int kant_index=0;kant_index<streit.ANZAHL_KANTEN;kant_index++)
            if(suchenSchwanzKnoten(streit,kant_index)==knote_index){
                if(start==0){
                    fprintf(dateizeiger, " 1");
                    start = 1;
                }else fprintf(dateizeiger, " 1");
            }else{
                if(start==0){
                    fprintf(dateizeiger, " 0");
                    start = 1;
                }else fprintf(dateizeiger, " 0");
            }
        if(knote_index==streit.ANZAHL_KNOTEN-1)
            fprintf(dateizeiger, "];\n");
        else fprintf(dateizeiger, ";\n");
    }
}

void flowConservationOutput(FILE* dateizeiger, struct Streit streit){
    fprintf(dateizeiger, "output_c =[\n");
    for(int knote_index=0;knote_index<streit.ANZAHL_KNOTEN;knote_index++){
        fprintf(dateizeiger, "\t");
        int start = 0;
        for(int kant_index=0;kant_index<streit.ANZAHL_KANTEN;kant_index++)
            if(suchenHeaderKnoten(streit,kant_index)==knote_index){
                if(start==0){
                    fprintf(dateizeiger, " 1");
                    start = 1;
                }else fprintf(dateizeiger, " 1");
            }else{
                if(start==0){
                    fprintf(dateizeiger, " 0");
                    start = 1;
                }else fprintf(dateizeiger, " 0");
            }
        if(knote_index==streit.ANZAHL_KNOTEN-1)
            fprintf(dateizeiger, "];\n");
        else fprintf(dateizeiger, ";\n");
    }
}

void linkcapacity(FILE* dateizeiger, struct Streit streit){
    fprintf(dateizeiger, "for e = 1:E\n");
    fprintf(dateizeiger, "\tlinear_constraint = sum(f_st(:,:,e)) <= alpha * C2;\n");
    // printf(dateizeiger, "\tlinear_constraint = sum(sum(f_st(:,:,e))) <= alpha * C2;\n");
    fprintf(dateizeiger, "\tConstraints = [Constraints, linear_constraint];\n");
    fprintf(dateizeiger, "end\n");
}

void flowCOnservation(FILE* dateizeiger){
    fprintf(dateizeiger, "for s = 1:N\n");
    fprintf(dateizeiger, "\tfor t = 1:N\n");
    fprintf(dateizeiger, "\t\tif s ~= t\n");
    fprintf(dateizeiger, "\t\t\tfor n = 1:N\n");
    fprintf(dateizeiger, "\t\t\t\tin_flow = f_st(s,t,:) .* input_c(n,:);\n");
    fprintf(dateizeiger, "\t\t\t\tout_flow = f_st(s,t,:) .* output_c(n,:);\n\n");
    fprintf(dateizeiger, "\t\t\t\tif n == s\n");
    fprintf(dateizeiger, "\t\t\t\t\tflow_conservation = (out_flow - in_flow == mu_st(s, t));\n");
    fprintf(dateizeiger, "\t\t\t\telseif n == t\n");
    fprintf(dateizeiger, "\t\t\t\t\tflow_conservation = (out_flow - in_flow == -mu_st(s, t));\n");
    fprintf(dateizeiger, "\t\t\t\telse n == t\n");
    fprintf(dateizeiger, "\t\t\t\t\tflow_conservation = (out_flow - in_flow == 0);\n");
    fprintf(dateizeiger, "\t\t\t\tend\n\n");
    fprintf(dateizeiger, "\t\t\t\tConstraints = [Constraints, flow_conservation];\n\n");
    fprintf(dateizeiger, "\t\t\tend\n");
    fprintf(dateizeiger, "\t\tend\n");
    fprintf(dateizeiger, "\tend\n");
    fprintf(dateizeiger, "end\n");
}

void lowerBound(FILE* dateizeiger){
    fprintf(dateizeiger, "Constraints = [Constraints, f_st(:) >= 0];\n");
}

void solver(FILE* dateizeiger){
    // fprintf(dateizeiger, "ops = sdpsettings('solver', 'sedumi');\n");
    // fprintf(dateizeiger, "sol = optimize(Constraints, Objective, ops);\n\n");
    fprintf(dateizeiger, "sol = optimize(Constraints, Objective);\n\n");

    fprintf(dateizeiger, "disp('alpha:');\n");
    fprintf(dateizeiger, "disp(value(alpha));\n");
}

void SOCP(FILE* dateizeiger){
    fprintf(dateizeiger, "for e = 1:E\n");
    fprintf(dateizeiger, "\tsocp_constraint = C1 * norm(sum(f_st(:,:,e) .* sigma_st), 2) <= C2 - sum(f_st(:,:,e) .* mu_st);\n");
    // fprintf(dateizeiger, "\t\t<= C2 - sum(sum(f_st(:,:,e) .* mu_st));\n");
    fprintf(dateizeiger, "\tConstraints = [Constraints, socp_constraint];\n");
    fprintf(dateizeiger, "end\n");
}
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



    // printsigma(sigma_kovarianz, streit);
    // // printsigma(mu_bedeutung, streit);
    // for(int knote_index=0;knote_index<ANZAHL_KNOTEN;knote_index++){
    //     // printInputEdge(streit, knote_index);
    //     // printOutputEdge(streit, knote_index);
    //     // printInputEdgeArray(streit, knote_index);
    //     printOutputEdgeArray(streit, knote_index);
    //     // printf("\n");
    // }


    // Öffnen Sie die Textdatei
	FILE *dateizeiger;
    dateizeiger = fopen(MATLAB_SKRIPT_WEG,"w+");

    addPath(dateizeiger);
    fprintf(dateizeiger, "\n");

    defineVariable(dateizeiger, streit);
    fprintf(dateizeiger, "\n");

    flowConservationInput(dateizeiger, streit);
    fprintf(dateizeiger, "\n");

    flowConservationOutput(dateizeiger, streit);
    fprintf(dateizeiger, "\n");

    defineConstant(dateizeiger, streit);
    fprintf(dateizeiger, "\n");

    defineMu(dateizeiger, streit, mu_bedeutung);
    fprintf(dateizeiger, "\n");

    defineSigma(dateizeiger, streit, sigma_kovarianz);
    fprintf(dateizeiger, "\n");

    fprintf(dateizeiger, "Objective = alpha;\n");
    fprintf(dateizeiger, "Constraints = [];\n");
    fprintf(dateizeiger, "\n");

    flowCOnservation(dateizeiger);
    fprintf(dateizeiger, "\n");

    linkcapacity(dateizeiger, streit);
    fprintf(dateizeiger, "\n");

    SOCP(dateizeiger);
    fprintf(dateizeiger, "\n");

    lowerBound(dateizeiger);
    fprintf(dateizeiger, "\n");

    solver(dateizeiger);
    fprintf(dateizeiger, "\n");
    // fprintf(dateizeiger, "N = %d\n", streit.ANZAHL_KNOTEN);
    // fprintf(dateizeiger, "E = %d\n", streit.ANZAHL_KANTEN);

//     double umgekehrt_Gaussian = gsl_cdf_gaussian_Pinv(ETA, 1.0);
// #ifdef DRUCKEN_ETA_MIT_UMGEKEHRT_GAUSSIAN
//     printf("Φ^{-1}(η) = "DRUCKEN_DOUBLE"\n", umgekehrt_Gaussian);
// #endif
//     // min ɑ
//     fprintf(dateizeiger, "Minimize\n");
//     fprintf(dateizeiger, "\tobj: "OBJEKTIV"\n");
    
//     fprintf(dateizeiger, "\nSubject to \n\n");

//     // \sum_{s,t\in N, s\neq t}\textcolor{blue}{f_{st}(e)}\mu_{st}  = \delta_e & \forall e\in E
//     for(int kant=1;kant<=ANZAHL_KANTEN;kant++){
//         fprintf(dateizeiger, DRUCKEN_ZWANG_ZAHL, einschränkungszähler);
//         for(int quelle=1;quelle<=ANZAHL_KNOTEN;quelle++)
//             for(int ziel=1;ziel<=ANZAHL_KNOTEN;ziel++)
//                 if(quelle!=ziel)
//                     if(quelle == ANZAHL_KNOTEN && ziel == ANZAHL_KNOTEN-1)
//                         fprintf(dateizeiger, DRUCKEN_DOUBLE" "PSD_VARIABLE" ", mu_bedeutung[quelle-1][ziel-1], quelle, ziel, kant);
//                     else fprintf(dateizeiger, DRUCKEN_DOUBLE" "PSD_VARIABLE" + ", mu_bedeutung[quelle-1][ziel-1], quelle, ziel, kant);

// #ifdef ALLE_GLEICHE_KAPAZITÄT  
//         fprintf(dateizeiger, " - "DELTA_VARIABLE" = "DRUCKEN_DOUBLE"\n", kant, KAPAZITÄT);
// #else
//         printf(KRED_L"FEHLER: FEHLT DIESES MODELL IST NOCH NICHT VOLLSTÄNDIG!\n"RESET);
//         return FEHLGESCHLAGEN;
// #endif
//         einschränkungszähler++;
//     }
//     fprintf(dateizeiger, "\n");

//     // [\sum_{s,t\in N, s\neq t}\Phi^{-1}(\eta)^2\sigma_{st}^2\textcolor{blue}{f_{st}(e)}^2-\delta_e^2]+2c_e\delta_e\leq c_e^2 & \forall e\in E
//     for(int kant=1;kant<=ANZAHL_KANTEN;kant++){
//         fprintf(dateizeiger, DRUCKEN_QUADRATISCH_ZWANG_ZAHL, quadratischschalter);
//         for(int quelle=1;quelle<=ANZAHL_KNOTEN;quelle++)
//             for(int ziel=1;ziel<=ANZAHL_KNOTEN;ziel++)
//                 if(quelle!=ziel){
//                     double koeffizient = (umgekehrt_Gaussian*umgekehrt_Gaussian*sigma_kovarianz[quelle-1][ziel-1]*sigma_kovarianz[quelle-1][ziel-1]);
//                     if(koeffizient>=0.0 && !(quelle==1 && ziel==2))
//                         fprintf(dateizeiger, "+ "DRUCKEN_DOUBLE" "PSD_VARIABLE"^2 ", koeffizient, quelle, ziel, kant);
//                     else fprintf(dateizeiger, DRUCKEN_DOUBLE" "PSD_VARIABLE"^2 ", koeffizient, quelle, ziel, kant);

// #ifdef DRUCKEN_WAHRSCHEINLICHKEITSKOEFFIZIENT
//                     printf("Für OD-Parr "DRUCKEN_INTEGER" ~ "DRUCKEN_INTEGER", koeffiient ist "DRUCKEN_DOUBLE"= umgekehrt_Gaussian("DRUCKEN_DOUBLE")^2*sigma("DRUCKEN_DOUBLE")^2\n", quelle, ziel, koeffizient, umgekehrt_Gaussian, sigma_kovarianz[quelle-1][ziel-1]);
// #endif

//                     if(quelle==ANZAHL_KNOTEN && ziel == ANZAHL_KNOTEN-1)
//                         fprintf(dateizeiger, " - "DELTA_VARIABLE"^2 ] ", kant);
//                 }

//         quadratischschalter++;

// #ifdef ALLE_GLEICHE_KAPAZITÄT
//         fprintf(dateizeiger, " <= 0\n");
// #else
//         printf(KRED_L"FEHLER: FEHLT DIESES MODELL IST NOCH NICHT VOLLSTÄNDIG!\n"RESET);
//         return FEHLGESCHLAGEN;
// #endif
//     }
//     fprintf(dateizeiger, "\n");

//     // \sum_{s,t\in N, s\neq t}\textcolor{blue}{f_{st}(e)}\leq \textcolor{blue}\alpha c_e & \forall e\in E
//     for(int kant=1;kant<=ANZAHL_KANTEN;kant++){
//         fprintf(dateizeiger, DRUCKEN_ZWANG_ZAHL, einschränkungszähler);
//         for(int quelle=1;quelle<=ANZAHL_KNOTEN;quelle++)
//             for(int ziel=1;ziel<=ANZAHL_KNOTEN;ziel++)
//                 if(quelle!=ziel){
//                     fprintf(dateizeiger, PSD_VARIABLE, quelle, ziel, kant);
//                     if(quelle==ANZAHL_KNOTEN && ziel == ANZAHL_KNOTEN-1)
//                         continue;
//                     else fprintf(dateizeiger, " + ");
//                 }
//         einschränkungszähler ++;
// #ifdef ALLE_GLEICHE_KAPAZITÄT
//         fprintf(dateizeiger, " - "DRUCKEN_DOUBLE OBJEKTIV" <= 0\n", KAPAZITÄT);
// #else
//         printf(KRED_L"FEHLER: FEHLT DIESES MODELL IST NOCH NICHT VOLLSTÄNDIG!\n"RESET);
//         return FEHLGESCHLAGEN;
// #endif
//     }
//     fprintf(dateizeiger, "\n");
    
//     // Flow conservation : constraints
//     schalter = 0;
//     for(int knote=1;knote<=ANZAHL_KNOTEN;knote++)
//         for(int quelle=1;quelle<=ANZAHL_KNOTEN;quelle++)
//             for(int ziel=1;ziel<=ANZAHL_KNOTEN;ziel++)
//                 if(quelle!=ziel){
//                     fprintf(dateizeiger, DRUCKEN_ZWANG_ZAHL, einschränkungszähler);
//                     for(int kant_index=0;kant_index<ANZAHL_KANTEN;kant_index++){
//                         if(suchenHeaderKnoten(streit,kant_index)==knote-1){
//                             if(schalter == 0)
//                                 schalter = 1;
//                             else if(schalter == 1)
//                                 fprintf(dateizeiger, " + ");
                            
//                             fprintf(dateizeiger, " "PSD_VARIABLE, quelle, ziel, kant_index+1);
//                         }

//                         if(suchenSchwanzKnoten(streit,kant_index)==knote-1){
//                             if(schalter == 0)
//                                 schalter = 1;
                            
//                             fprintf(dateizeiger, " - ");
//                             fprintf(dateizeiger, " "PSD_VARIABLE, quelle, ziel, kant_index+1);
//                         } 
//                     }

//                     if(knote==quelle)
//                         fprintf(dateizeiger, " =  "DRUCKEN_DOUBLE"\n", mu_bedeutung[quelle-1][ziel-1]);
//                     else if(knote==ziel)
//                         fprintf(dateizeiger, " = -"DRUCKEN_DOUBLE"\n", mu_bedeutung[quelle-1][ziel-1]);
//                     else  fprintf(dateizeiger, " = 0\n");
                    
//                     schalter = 0;
//                     einschränkungszähler++;
//                 }
//     fprintf(dateizeiger, "\n");
	
//     fprintf(dateizeiger, "\nBounds\n\n"); 

//     // \textcolor{blue}{f_{st}(e)}\geq0 & \forall s,t\in N\ s\neq t\ ,e\in E
//     for(int kant=1;kant<=ANZAHL_KANTEN;kant++)
//         for(int quelle=1;quelle<=ANZAHL_KNOTEN;quelle++)
//             for(int ziel=1;ziel<=ANZAHL_KNOTEN;ziel++)
//                 if(quelle!=ziel)
//                     fprintf(dateizeiger, "\t"PSD_VARIABLE" >= 0\n", quelle, ziel, kant);
//     fprintf(dateizeiger, "\n");

//     for(int kant=1;kant<=ANZAHL_KANTEN;kant++)
//         fprintf(dateizeiger, "\t"DELTA_VARIABLE" >= 0\n", kant);

//     fprintf(dateizeiger, "\t"OBJEKTIV" >= 0\n");

//     fprintf(dateizeiger, "\nEnd\n");
    fclose(dateizeiger);

//     // Erhalten Sie die Lösung
//     pfostenHaupt(streit);

//     // Analysieren
//     double alpha = erhaltenALPHA(streit);
//     double*** bruchsarray = erhaltenPSDBurch(streit);

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