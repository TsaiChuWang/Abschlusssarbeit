
#define ETA 0.9

// Link capacity
#define ALLE_GLEICHE_KAPAZITÄT
// #define KANT_UNTERSCHIEDLICHE_KAPAZITÄT
// #define MODUS_GRENZE_VERKHERSEICHWITE 

// Demand interval model 
#define MODUS_GRENZE_VERKHERSEICHWITE 1

// Decision Variable Name
#define OBJEKTIV        "ALPHA"
#define PSD_VARIABLE    "f_%02d_%02d(e%02d)"
#define DELTA_VARIABLE  "delta_e%02d"

//Show the data
// #define STREIT_DRUCKEN                              "[Haupt.c] main Display the detail of the topology"
// #define DRUCKEN_ETA_MIT_UMGEKEHRT_GAUSSIAN          "[Haupt.c] main Print the inserve Gaussian function of defined η "
// #define DRUCKEN_WAHRSCHEINLICHKEITSKOEFFIZIENT      "[Haupt.c] main Display the coefficient of the first constraint"
#define NUR_TRANSFER_VERKEHR_BEOBACHTUNG            "[Haupt.c] main Only the transfer are observation"

// #define DRUCKEN_DATEI_LÄNGE                         "[Netz.h] lesenDatei Print the length of the file."
// #define DRUCKEN_DATEI_INHALT                        "[Netz.h] pfostenHaupt Show the content in the file"
// #define DRUCKEN_ANTWORTEN_GESPEICHERT               "[Netz.h] pfostenHaupt Respond has been stored."
// #define DRUCKEN_GEPOSTETE_INHALTE                   "[Netz.h] pfostenHaupt Posted content."
// #define DRUCKEN_LÖSUNG_ERSTELLTMODELLFORTSCHRITT    "[Netz.h] pfostenHaupt Reminder the solution has been saved."

// #define DRUCKEN_MU_BEDEUTUNG_IM_ERHALTEN            "[Gaussian.h] erhaltenBedeutung Print the matrix consists of mean of dst."
// #define DRUCKEN_SIGMA_KOVARIANZ_IM_ERHALTEN         "[Gaussian.h] erhaltenBedeutung Print the matrix consists of covariance of dst."

#define DRUCKEN_ERHALTET_ALPHA                      "[Parser.h] erhaltenALPHA Print the alpha value." 
// #define DRUCKEN_ERHALTENPSDBRUCH_WERT               "[Parser.h] erhaltenPSDBurch Print the routing fractions." 
// #define DRUCKEN_ERHALTENWERT_WERT_SCHNUR            "[Parser.h] erhaltenWert Print the extracted substring." 
// #define DRUCKEN_ERHALTENWERT_WERT                   "[Parser.h] erhaltenWert Print the extracted value from the substring."