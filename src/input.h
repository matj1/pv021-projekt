#ifndef PV021_PROJEKT_INPUT_H
#define PV021_PROJEKT_INPUT_H

#include <stddef.h>
#include <stdio.h>

typedef struct {
	float *data;
	size_t velikost;
} pole_t;

/**
 * Velikost bufferu pro načitání souboru
 */
#define VELIKOST_BUFFERU (1 << 16)
#define VELIKOST_OBRAZKU (28 * 28)

/**
 * Toto bere deskriptor souboru se vstupními daty a vrací pole vstupních obrAzku.
 * Každý vstupní obrAzek je pole floatu.
 *
 * @param soubor ukazatel na soubor budoucí cteným
 * @return pole zpracovaných vstupních dat
 */
pole_t nacist_data(FILE *soubor);

int *nacist_cile(FILE *soubor, int pocet_radku);

/**
 * Toto precte daný soubor a vrAtí pocet rAdku v něm.
 * Toto vrAtí ukazatel pozice v tom souboru na zacAtek.
 *
 * @param soubor
 * @return pocet rAdku
 */
size_t secti_radky(FILE *soubor);

// void bin_parsuj_vstupy(float *vstup, FILE *odkud, char **buff) {
//  for (int i = 1; i < VSTUPU; ++i) {
//    vstup[i - 1] = vstup[i];
//  }
//  unsigned int size = 20;
//  getline(buff, &size, odkud);
//  vstup[VSTUPU - 1] = atof(*buff);
//  return;
//}
//
// void upload_koef(float **prvni_koef, float prvni_tr, float *druhy_koef,
//                 float druhy_tr, FILE *kam) {
//  for (int i = 0; i < NEU; ++i) {
//    for (int j = 0; j < VSTUPU; ++j) {
//      fprintf(kam, "%.4f\n", prvni_koef[j][i]);
//    }
//    fprintf(kam, "%.4f\n", prvni_tr[i]);
//    fprintf(kam, "%.4f\n", druhy_koef[i]);
//  }
//  fprintf(kam, "%.4f\n", druhy_tr);
//  return;
//}
//
// void import_koef(int vrstev, int *pocty, double ***vaha, FILE *odkud);

#endif // PV021_PROJEKT_INPUT_H
