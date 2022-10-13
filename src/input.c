#include "input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Toto bere deskriptor souboru se vstupnimi daty a vraci pole vstupnich obrazku.
 * Každy vstupni obrazek je pole floatu.
 *
 * @param soubor ukazatel na soubor budouci ctenym
 * @return pole zpracovanych vstupnich dat; každy prvek je jeden obrazek a ma VELIKOST_OBRAZKU prvku
 */
pole_t nacist_data(FILE *soubor) {
	size_t pocet_radku = secti_radky(soubor);
	char buffer[VELIKOST_BUFFERU] = {0};
	float *data = malloc(pocet_radku * (VELIKOST_OBRAZKU + 1) * sizeof(float));

	size_t byty_precteny;
	size_t delka_radku = VELIKOST_OBRAZKU * 4;
	char *radek = malloc(delka_radku);

	// hlavni smycka nacitani dat
	size_t pocet_prectenych_radku = 0;
	while (!feof(soubor)) {
		getline(&radek, &delka_radku, soubor);

		size_t pocet_prectenych_cisel = 0;
		char *cislo = radek;
		while (1) {
			cislo = strtok(cislo, ",");
			if (cislo == NULL) {
				break;
			}

			data[pocet_prectenych_radku * (delka_radku + 1) + pocet_prectenych_cisel] =
			      atof(cislo);
			pocet_prectenych_cisel++;
		}

		pocet_prectenych_radku++;
	}
	free(radek);
	return (pole_t){.velikost = pocet_radku, .data = data};
}

/**
 * nacte cile :D
 *
 *
 *
 *
 *
 */
int *nacist_cile(FILE *soubor, int pocet_radku) {
	char buffer[VELIKOST_BUFFERU] = {0};
	int *data = malloc(pocet_radku * sizeof(int));

	size_t byty_precteny;
	size_t delka_radku = 2;
	char *radek = malloc(delka_radku);

	// hlavni smycka nacitani dat
	size_t pocet_prectenych_radku = 0;
	for (int i = 0; i < pocet_radku; ++i) {
		getline(&radek, &delka_radku, soubor);
		data[pocet_prectenych_radku] = radek[0]-48;

		pocet_prectenych_radku++;
	}

	return data;
}

/**
 * Toto precte dany soubor a vrati pocet radku v něm.
 * Toto vrati ukazatel pozice v tom souboru na zacatek.
 *
 * @param soubor
 * @return pocet radku
 */
size_t secti_radky(FILE *soubor) {
	char buffer[VELIKOST_BUFFERU] = {0};
	size_t pocet_radku = 0;
	size_t byty_precteny;

	do {
		byty_precteny = fread(buffer, 1, VELIKOST_BUFFERU, soubor);

		for (size_t p = 0; p < VELIKOST_BUFFERU; p++) {
			if (buffer[p] == '\n') {
				pocet_radku++;
			}
		}
	} while (!feof(soubor));

	if (buffer[byty_precteny - 1] != '\n') { // pro zapocitani radku na konci bez \n
		pocet_radku++;
	}

	rewind(soubor);
	return pocet_radku;
}

/*
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
*/
