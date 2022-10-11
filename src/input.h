//
// Created by matj1 on 11.10.22.
//

#ifndef PV021_PROJEKT_INPUT_H
#define PV021_PROJEKT_INPUT_H

typedef struct {
	float *data;
	size_t velikost;
} pole_t;

/**
 * Velikost bufferu pro načítání souboru
 */
#define VELIKOST_BUFFERU (1 << 16)
#define VELIKOST_OBRÁZKU (28 * 28)

/**
 * Toto bere deskriptor souboru se vstupními daty a vrací pole vstupních obrázků.
 * Každý vstupní obrázek je pole floatů.
 *
 * @param soubor ukazatel na soubor budoucí čteným
 * @return pole zpracovaných vstupních dat
 */
pole_t načíst_data(FILE *soubor);

int *načíst_cíle(FILE *soubor, int počet_řádků);

/**
 * Toto přečte daný soubor a vrátí počet řádků v něm.
 * Toto vrátí ukazatel pozice v tom souboru na začátek.
 *
 * @param soubor
 * @return počet řádků
 */
size_t sečti_řádky(FILE *soubor);

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
