#include "input.h"
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VSTUPU 784
#define VYSTUPU 10
#define SKLON 0.005
#define RYCHLOST 0.007 // když je moc velká, tak celá neuronka spadne do předpovídání jediné
	                // kategorie
#define DELKA_UCENI 5000
#define BEC 10
#define P_VLAAKEN 10 // pořešit případ BEC!=P_VLAAKEN

int vrstvy;
int *pocsty;
float ***vaaha;
float ***neu;
float ***vyysledky;
float ***derivace;
int odhady_vlaaken[P_VLAAKEN];
int *ciile_vlaaken;

pthread_t threads[P_VLAAKEN];
int thread_args[P_VLAAKEN];
int rc;

float *prziiklady;
int *ciile;

// za sebe mluvící různé pokusné aktivační funkce, použije se nejspíš relu

float sigmoida(float x) { return 1 / (1 + exp(-x)); }

float der_sigmoida(float x) { return sigmoida(x) * (1 - sigmoida(x)); }

float relu(float x) {
	if (x > 0) {
		return x;
	}
	return 0; // SKLON * x; //?
}

float der_relu(float x) {
	if (x > 0) {
		return 1;
	}
	return SKLON; // 0; //?
}

/*
iterace provede jeden průchod neuronovou siti a vrati čislo uhodnuté kategotie

float ***vaaha - vaaha[i][j][k] vaaha spoje mezi neuronem ve vrstve i indexu j a neuronem ve vrstve
i+1 a indexem k

float **vyysledky vyysledky[i][0] je 784 vstupů +1 threshold, neni třeba
inicializace

float **neu neinicializované pole pro všechny neurony mimo thresholdy

int vrstvy počet skrytých vrstev

int *pocsty počet neuronů v každé vrstve bez thresholdů
*/

int iterace(float **vyysledky_jednoho, float **neu_jednoho) {
	for (int i = 0; i < vrstvy;
	     ++i) { // vrstev je vrstvy +2 (posledni index je vrstvy+1), ale posledni je výstupni
		vyysledky_jednoho[i][pocsty[i]] = 1;      // nastaveni thresholdového neuronu
		for (int k = 0; k < pocsty[i + 1]; ++k) { // pro každý neuron vyšši vrstvy
			neu_jednoho[i + 1][k] = 0;                  // vynulovani na začatku sumovani
			for (int j = 0; j < pocsty[i] + 1;
			     ++j) { // pro každý neuron aktualni vrstvy +1 aby se počitalo i s thresholdem
				neu_jednoho[i + 1][k] += vyysledky_jednoho[i][j] * vaaha[i][j][k];
			}
			vyysledky_jednoho[i + 1][k] =
			      relu(neu_jednoho[i + 1][k]); // otazka vice aktivacnich funkci
		}
	}

	//řešeni výstupni vrstvy
	int max = 7;
	neu_jednoho[vrstvy + 1][7] = 0;
	vyysledky_jednoho[vrstvy][pocsty[vrstvy]] = 1; // nastaveni thresholdu
	for (int i = 0; i < VYSTUPU; ++i) {
		neu_jednoho[vrstvy + 1][i] = 0;
		for (int j = 0; j < pocsty[vrstvy] + 1; ++j) {
			neu_jednoho[vrstvy + 1][i] += vyysledky_jednoho[vrstvy][j] * vaaha[vrstvy][j][i];
		}
		if (neu_jednoho[vrstvy + 1][i] > neu_jednoho[vrstvy + 1][max]) { // odecitani kvuli preteceni
			max = i;
		}
	}

	float suma = 0;
	for (int i = 0; i < VYSTUPU; ++i) {
		suma += exp(neu_jednoho[vrstvy + 1][i] - neu_jednoho[vrstvy + 1][max]); // odecitani kvuli preteceni
	}
	for (int i = 0; i < VYSTUPU; ++i) {
		vyysledky_jednoho[vrstvy + 1][i] = exp(neu_jednoho[vrstvy + 1][i] - neu_jednoho[vrstvy + 1][max]) /
		                                   suma; // odecitani kvuli preteceni
	}
	return max;
}

void bekpropagejsn(float **vyysledky_jednoho, float **derivace_jednoho, float **neu_jednoho, int ciil, int *odhad) {
	int vyysledek = iterace(vyysledky_jednoho,neu_jednoho);
	*odhad = vyysledek;
	for (int i = 0; i < VYSTUPU; ++i) {
		if (i != ciil) {
			derivace_jednoho[vrstvy + 1][i] = vyysledky_jednoho[vrstvy + 1][i];
		}
	}
	derivace_jednoho[vrstvy + 1][ciil] = vyysledky_jednoho[vrstvy + 1][ciil] - 1;
	for (int i = vrstvy; i > 0; --i) {
		for (int j = 0; j < pocsty[i]; ++j) {
			derivace_jednoho[i][j] = 0;
			for (int k = 0; k < pocsty[i + 1]; ++k) {
				derivace_jednoho[i][j] +=
				      vaaha[i][j][k] * der_relu(neu_jednoho[i][j]) * derivace_jednoho[i + 1][k];
			}
		}
	}
	return;
}

void *jedno_vlaakno_bekpropagejsn(void *i) {
	bekpropagejsn(vyysledky[*((int *)i)], derivace[*((int *)i)], neu[*((int *)i)], ciile_vlaaken[*((int *)i)], &odhady_vlaaken[*((int *)i)]);
	return NULL;
}

int trenink(int pozice, int **odhady) {
	int spraavnje = 0;

	ciile_vlaaken = &ciile[pozice];
	// TODO tohle je tvoje parketa na paralelizaci, už se na ni těším

	for (int i = 0; i < BEC; ++i) { // TODO i<P_VLAAKEN
		vyysledky[i][0] = &prziiklady[(pozice + i) * (VSTUPU + 1)];
		thread_args[i] = i;
		rc = pthread_create(&threads[i], NULL, jedno_vlaakno_bekpropagejsn,
		                    (void *)&thread_args[i]);
	}
	for (int i=0; i<P_VLAAKEN; ++i) {
    		rc = pthread_join(threads[i], NULL);
  	}
	for (int i = 0; i < P_VLAAKEN; ++i) {
		spraavnje += odhady_vlaaken[i] == ciile_vlaaken[i];
		odhady[ciile_vlaaken[i]][odhady_vlaaken[i]]+=1;
	}
	// změna vah
	for (int i = 0; i < BEC; ++i) {
		// printf("%d beč\n",i);
		for (int j = 0; j < vrstvy + 1; ++j) {
			for (int l = 0; l < pocsty[j + 1]; ++l) {
				for (int k = 0; k < pocsty[j] + 1; ++k) {
					vaaha[j][k][l] -= derivace[i][j + 1][l] * vyysledky[i][j][k] *
					                  RYCHLOST; // RYCHLOST jako funkce nečeho ?
				}
			}
		}
	}
	return spraavnje;
}

int main(int argc, char **argv) {

	// deklarace všech velkých poli

	vrstvy = atoi(argv[1]); // uzivatel zada pocet skrytych (ne vstupnich, ne
	                        // vystupnich) vrstev
	pocsty = malloc((vrstvy + 2) * sizeof(int));
	pocsty[0] = VSTUPU;
	pocsty[vrstvy + 1] = VYSTUPU;
	printf("%d\n", pocsty[0]);
	for (int i = 1; i <= vrstvy; ++i) {
		pocsty[i] = atoi(argv[i + 1]); // uzivatel zada pocsty skrytych neuronu jako
		                               // dalsi argumenty main (tolik cisel, kolik
		                               // deklaroval, ze bude zadavat)
		printf("%d\n", pocsty[i]);
	}
	printf("%d\n", pocsty[vrstvy + 1]);
	vaaha = malloc((vrstvy + 1) * sizeof(float **));
	srand(2);
	for (int j = 0; j < vrstvy + 1; ++j) {
		vaaha[j] = malloc((pocsty[j] + 1) * sizeof(float *));
		for (int k = 0; k < pocsty[j] + 1; ++k) {
			vaaha[j][k] = malloc(pocsty[j + 1] * sizeof(float));
			for (int g = 0; g < pocsty[j + 1]; ++g) {
				vaaha[j][k][g] =
				      0.1 * (float)rand() / (float)RAND_MAX - 0.05; //
				                                                    // inicializace vah
			}
		}
	}
	neu=malloc(BEC*sizeof(float **));
	vyysledky = malloc((BEC) * sizeof(float **));
	derivace = malloc((BEC) * sizeof(float **));

	for (int k = 0; k < BEC; ++k) {
		vyysledky[k] = malloc((vrstvy + 2) * sizeof(float *));
		for (int i = 0; i < vrstvy + 2; ++i) {
			vyysledky[k][i] = malloc((pocsty[i] + 1) *
			                         sizeof(float)); // vim, ze neni potreba alokovat
			                                         // threshold vystupni vrstvy, ale co uz
		}
	}

	for (int k = 0; k < BEC; ++k) {
		derivace[k] = malloc((vrstvy + 2) * sizeof(float *));
		neu[k]=malloc((vrstvy+2)*sizeof(float *));
		for (int i = 1; i < vrstvy + 2; ++i) {
			derivace[k][i] = malloc(pocsty[i] * sizeof(float));
			neu[k][i] = malloc(pocsty[i] *sizeof(float));
		}
	}
	// načteni dat a trénink
	// TODO pořešit cesty k datům
	// velikost trénovacího souboru by měla být násobkem BEC
	FILE *vstup = fopen("../../data/train_vectors.csv", "r");
	FILE *validacni_vektor = fopen("../../data/val_train_vectors.csv", "r");

	printf("vstupy ok\n");
	FILE *vyystupy = fopen("../../data/train_labels.csv", "r");
	FILE *validacni_popisek = fopen("../../data/val_train_labels.csv", "r");

	printf("vystupy ok\n");
	int deelka = secti_radky(vyystupy);
	int val_deelka = secti_radky(validacni_popisek);

	prziiklady = nacist_data(vstup, deelka);
	float *val_prziikady = nacist_data(validacni_vektor, val_deelka);

	printf("nacitam cile\n");
	ciile = nacist_cile(vyystupy, deelka);
	int *val_ciile = nacist_cile(validacni_popisek, val_deelka);

	int spraavnje;
	int *odhady[10]; // confusion matrix

	FILE *ven = fopen("vahy2.txt", "w");
	printf("zacatek treninku\n");
	for (int i = 0; i < 10; ++i) {
		odhady[i] = malloc(10 * sizeof(int));
	}

	for (int p = 0; p < 50; ++p) {
		for (int i = 0; i < 10; ++i) {
			memset(odhady[i], 0, 10 * sizeof(int));
		}

		spraavnje = 0;
		for (int i = 0; i < deelka - BEC + 1; i += BEC) {
			spraavnje += trenink(i, odhady); //to největší počítání
		}

		for (int v = 0; v < 10; ++v) { // confusion matrix
			for (int w = 0; w < 10; ++w) {
				printf("%8d", odhady[v][w]);
			}
			printf("\n");
		}
		printf("%d. kolo %d\n", p, spraavnje);

		printf("validace\n");
		spraavnje = 0;

		for (int i = 0; i < 10; ++i) {
			memset(odhady[i], 0, 10 * sizeof(int));
		}
		int vyysledek;
		for (int i = 0; i < val_deelka; ++i) {
			vyysledky[0][0] = &val_prziikady[i * (VSTUPU + 1)];
			vyysledek = iterace(vyysledky[0],neu[0]);
			spraavnje += val_ciile[i] == vyysledek;
			odhady[val_ciile[i]][vyysledek] += 1;
		}

		for (int v = 0; v < 10; ++v) { // confusion matrix
			for (int w = 0; w < 10; ++w) {
				printf("%8d", odhady[v][w]);
			}
			printf("\n");
		}
		printf("validace po %d. kole %d\n", p, spraavnje);
	}
	vypis_vahy(ven, vaaha, pocsty, vrstvy);
	free(prziiklady);
	return 0;
}
/*
příklad spuštění: ./projekt 2 40 15
*/

/* Přehled všeho, co ješte neni dořešené:
1. Kde budou data relativne ke kódu
2. Nepouživaný nultý pointer v neu a derivace
3. Vybrat spravnou architekturu
4. Vybrat spravnou aktivačni funkci / vice aktivačnich funkci?
5. OK --- Maji se upravit vahy po každém přikladu, nebo až po n-tici přikladů?
6. OK --- Konečne by se to melo spustit
7. Vyřešit, jestli nebude rychlost učeni na nečem závislá.


*/
