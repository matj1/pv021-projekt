//
// Created by matj1 on 11.10.22.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "input.h"

#define VELIKOST_BUFFERU (1<<16)
#define VELIKOST_OBRÁZKU (28*28)

/**
 * Toto bere deskriptor souboru se vstupními daty a vrací pole vstupních obrázků.
 * Každý vstupní obrázek je pole floatů.
 *
 * @param soubor ukazatel na soubor budoucí čteným
 * @return pole zpracovaných vstupních dat; každý prvek je jeden obrázek a má VELIKOST_OBRÁZKU prvků
 */
pole_t načíst_data(FILE* soubor) {
	size_t počet_řádků = sečti_řádky(soubor);
	char buffer[VELIKOST_BUFFERU] = {0};
	float* data = malloc(počet_řádků * VELIKOST_OBRÁZKU * sizeof(float));

	size_t byty_přečteny;
	size_t délka_řádku;
	char* řádek = malloc(délka_řádku);

	// hlavní smyčka načítání dat
	size_t počet_přečtených_řádků = 0;
	while (!feof(soubor)) {
		getline(&řádek, &délka_řádku, soubor);

		size_t počet_přečtených_čísel = 0;
		char* číslo = řádek;
		while (1) {
			číslo = strtok(číslo, ",");
			if (číslo == NULL) {
				break;
			}

			data[počet_přečtených_řádků*délka_řádku + počet_přečtených_čísel] = atof(číslo);
			počet_přečtených_čísel++;
		}

		počet_přečtených_řádků++;
	}

	return (pole_t){.velikost = počet_řádků, .data = data};
}

/**
 * Toto přečte daný soubor a vrátí počet řádků v něm.
 * Toto vrátí ukazatel pozice v tom souboru na začátek.
 *
 * @param soubor
 * @return počet řádků
 */
size_t sečti_řádky(FILE* soubor) {
	char buffer[VELIKOST_BUFFERU] = {0};
	size_t počet_řádků = 0;
	size_t byty_přečteny;

	do {
		byty_přečteny = fread(buffer, 1, VELIKOST_BUFFERU, soubor);

		for (size_t p = 0; p < VELIKOST_BUFFERU; p++) {
			if (buffer[p] == '\n') {
				počet_řádků++;
			}
		}
	} while (!feof(soubor));

	if ([byty_přečteny - 1] != '\n') { // pro započítání řádku na konci bez \n
		počet_řádků++;
	}

	rewind(soubor);
	return  počet_řádků;
}


void bin_parsuj_vstupy(float *vstup, FILE *odkud, char **buff) {
  for (int i = 1; i < VSTUPU; ++i) {
    vstup[i - 1] = vstup[i];
  }
  unsigned int size = 20;
  getline(buff, &size, odkud);
  vstup[VSTUPU - 1] = atof(*buff);
  return;
}

void upload_koef(float **prvni_koef, float prvni_tr, float *druhy_koef,
                 float druhy_tr, FILE *kam) {
  for (int i = 0; i < NEU; ++i) {
    for (int j = 0; j < VSTUPU; ++j) {
      fprintf(kam, "%.4f\n", prvni_koef[j][i]);
    }
    fprintf(kam, "%.4f\n", prvni_tr[i]);
    fprintf(kam, "%.4f\n", druhy_koef[i]);
  }
  fprintf(kam, "%.4f\n", druhy_tr);
  return;
}

void import_koef(int vrstev, int *pocty, double ***vaha, FILE *odkud) {
  for (int i = 0; i < vrstev + 1; ++i) {
    for (int j = 0; j < pocty[i]; ++j) {
      for (int k = 0; k < pocty[i + 1]; ++k) {

        getline(buff, &size, odkud);
        prvni_koef[j][i] = atof(*buff);
      }
    }
    getline(buff, &size, odkud);
    prvni_tr[i] = atof(*buff);
    getline(buff, &size, odkud);
    druhy_koef[i] = atof(*buff);
  }
  getline(buff, &size, odkud);
  druhy_tr = atof(*buff);
  free(*buff);
  return;
}