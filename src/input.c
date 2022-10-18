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
float *nacist_data(FILE *soubor, int pocet_radku) {
	float *data = malloc(pocet_radku * (VELIKOST_OBRAZKU + 1) * sizeof(float));

	size_t delka_radku = VELIKOST_OBRAZKU * 4 * sizeof(char);
	char *radek = malloc(delka_radku);

	printf("nacitam %d radku\n", pocet_radku); // debug

	// hlavni smycka nacitani dat
	for (int i = 0; i < pocet_radku; ++i) {
		// printf("%d\n",i); //debug
		getline(&radek, &delka_radku, soubor);
		// printf("%s\n",radek); //debug
		size_t pocet_prectenych_cisel = 0;
		char *cislo = strtok(radek, ",");
		while (cislo != NULL) {
			// printf("%s ",cislo); //debug
			data[i * (VELIKOST_OBRAZKU + 1) + pocet_prectenych_cisel] = atof(cislo)/255.0;
			pocet_prectenych_cisel++;
			cislo = strtok(NULL, ",");
		}
	}
	free(radek);
	printf("nacitani ok\n"); // debug
	return data;
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
	int *data = malloc(pocet_radku * sizeof(int));

	size_t delka_radku = 2 * sizeof(char);
	char *radek = malloc(delka_radku);

	// hlavni smycka nacitani dat
	for (int i = 0; i < pocet_radku; ++i) {
		getline(&radek, &delka_radku, soubor);
		data[i] = radek[0] - 48;
	}
	free(radek);
	return data;
}

/**
 * Toto precte dany soubor a vrati pocet radku v něm.
 * Toto vrati ukazatel pozice v tom souboru na zacatek.
 *
 * @param soubor
 * @return pocet radku
 */
int secti_radky(FILE *soubor) { // rvat sem jenom cile
	size_t velikost = 2;
	char *buffer = malloc(velikost);
	int pocet_radku = 0;
	while (!feof(soubor)) {
		getline(&buffer, &velikost, soubor);
		pocet_radku++;
	}
	rewind(soubor);
	free(buffer);
	return pocet_radku - 1;
}

void vypis_vahy(FILE *ven, float ***vaha, int *pocty, int vrstvy) {
	for (int i = 0; i < vrstvy + 1; ++i) {
		for (int j = 0; j < pocty[i + 1]; ++j) {
			fprintf(ven,"%d. neuron %d. vrstvy\n",j,i+1);
			for (int k = 0; k < pocty[i] + 1; ++k) {
				fprintf(ven, "%f; ", vaha[i][k][j]);
			}
			fprintf(ven, "\n");
		}
		fprintf(ven, "\n");
	}
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
