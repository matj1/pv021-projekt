#include "input.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define VSTUPU 784
#define VYSTUPU 10
#define SKLON 0.05
#define RYCHLOST 2
#define DELKA_UCENI 5000

// za sebe mluvící různé pokusné aktivační funkce, použije se nejspíš relu

float sigmoida(float x) { return 1 / (1 + exp(-x)); }

float der_sigmoida(float x) { return sigmoida(x) * (1 - sigmoida(x)); }

float relu(float x) {
	if (x > 0) {
		return x;
	}
	return 0; // SKLON * x;
}

float der_relu(float x) {
	if (x > 0) {
		return 1;
	}
	return SKLON; // 0;
}

/*
iterace provede jeden průchod neuronovou sítí a vrátí číslo uhodnuté kategotie

float ***vaha - vaha[i][j][k] váha spoje mezi neuronem ve vrstvě i indexu j a neuronem ve vrstvě
i+1 a indexem k

float **vysledky vysledky[0] je 784 vstupů +1 threshold, jinak není třeba
inicializace

float **neu neinicializované pole pro všechny neurony mimo thresholdy

int vrstvy počet skrytých vrstev

int *pocty počet neuronů v každé vrstvě bez thresholdů
*/

int iterace(float ***vaha, float **vysledky, float **neu, int vrstvy, int *pocty) {
	for (int i = 0; i < vrstvy; ++i) { // vrstev je vrstvy +2 (poslední index je vrstvy+1), ale poslední je výstupní
		vysledky[i][pocty[i]] = 1;       // nastavení thresholdového neuronu
		for (int k = 0; k < pocty[i + 1]; ++k) {       // pro každý neuron vyšší vrstvy
			neu[i + 1][k] = 0;                       // vynulování na začátku sumování
			for (int j = 0; j < pocty[i] + 1; ++j) { // pro každý neuron aktuální vrstvy +1 aby se počítalo i s thresholdem
				neu[i + 1][k] += vysledky[i][j] * vaha[i][j][k];
			}
			vysledky[i + 1][k] = relu(neu[i + 1][k]); // otazka vice aktivacnich funkci
		}
	}

	//řešení výstupní vrstvy
	int max = 0;
	for (int i = 0; i < VYSTUPU; ++i) {
		neu[vrstvy + 1][i] = 0;
		vysledky[vrstvy][pocty[vrstvy]] = 1; // nastavení thresholdu
		for (int j = 0; j < pocty[vrstvy] + 1; ++j) {
			neu[vrstvy + 1][i] += vysledky[vrstvy][j] * vaha[vrstvy][j][i];
		}
		/*if (neu[vrstvy + 1][i] > neu[vrstvy+1][max]) {
		      max = i;
		}*/
	}
	float suma = 0;
	for (int i = 0; i < VYSTUPU; ++i) {
		suma += exp(neu[vrstvy + 1][i]); //- neu[vrstvy+1][max]);
	}
	for (int i = 0; i < VYSTUPU; ++i) {
		vysledky[vrstvy + 1][i] = exp(neu[vrstvy + 1][i]) / suma; // - max) / suma;
	}

	return max;
}

int trenink(float ***vaha, float **neu, float **vysledky, float **derivace, int vrstvy, int *pocty,
            int cil) {
	int vysledek = iterace(vaha, vysledky, neu, vrstvy, pocty);

	for (int i = 0; i < VYSTUPU; ++i) {
		if (i != cil) {
			derivace[vrstvy + 1][i] =
			      vysledky[vrstvy + 1]
			              [i]; // anebo += , příprava na přístup několik příkladů: jedno učení
		}
	}
	derivace[vrstvy + 1][cil] = 1 - vysledky[vrstvy + 1][cil]; // taky nebo +=

	for (int i = vrstvy; i > 0; --i) {
		for (int j = 0; j < pocty[i]; ++j) {
			derivace[i][j] = 0; // přesunout do nadřazené funkce ?
			for (int k = 0; k < pocty[i + 1]; ++k) {
				derivace[i][j] += vaha[i][j][k] * der_relu(neu[i][j]) * derivace[i + 1][k];
			}
		}
	}

	for (int i = 0; i < vrstvy + 1; ++i) {
		for (int j = 0; j < pocty[i] + 1; ++j) {
			for (int k = 0; k < pocty[i + 1]; ++k) {
				vaha[i][j][k] -= derivace[i + 1][k] * vysledky[i][j] *
				                 RYCHLOST; // RYCHLOST jako funkce něčeho ?
			}
		}
	}
	return vysledek == cil; // můžeme potom sečíst výsledky tréninku pro celkový počet správně
	                        // odhadnutých číslic
}

int main(int argc, char **argv) {

	// deklarace všech velkých polí

	int vrstvy = atoi(argv[1]); // uzivatel zada pocet skrytych (ne vstupnich, ne
	                            // vystupnich) vrstev
	int pocty[vrstvy + 2];
	pocty[0] = VSTUPU;
	pocty[vrstvy + 1] = VYSTUPU;

	for (int i = 1; i <= vrstvy; ++i) {
		pocty[i] = atoi(argv[i + 1]); // uzivatel zada pocty skrytych neuronu jako
		                              // dalsi argumenty main (tolik cisel, kolik
		                              // deklaroval, ze bude zadavat)
	}

	float **vaha[vrstvy + 1];
	srand(0);
	for (int j = 0; j < vrstvy + 1; ++j) {
		float vrstva[pocty[j]][pocty[j + 1]];
		for (int k = 0; k < pocty[j] + 1; ++k) {
			for (int g = 0; g < pocty[j + 1]; ++g) {
				vrstva[k][g] = 2 * (float)rand() / (float)RAND_MAX -
				               1; // inicializace vah mezi -1 a 1
			}
		}
		vaha[j] = vrstva;
	}

	float *neu[vrstvy + 2]; // neu[0] ale nepoužívám (přehlednost ?)
	float *vysledky[vrstvy + 2];
	float *derivace[vrstvy + 2]; // derivace[0] taky nepoužívám

	// neu[0]= float neurony[VSTUPU];
	// vysledky[0] = float vysledek[VSTUPU];

	for (int j = 1; j < vrstvy + 2; ++j) {
		float neurony[pocty[j]];
		float vysledek[pocty[j] + 1];
		float der[pocty[j]];
		// memset(neurony, 0, pocty[j]); //není třeba
		neu[j] = neurony; // neu[j-1] ? není potřeba sumovat vstupní vrstvu
		vysledky[j] = vysledek;
		derivace[j] = der; // nebo der[j-1] ? nepouzivam derivaci prvni vstupni vrstvy
	}

	/*
	if (argc > i) {
	  FILE *koef;
	  koef = fopen(argv[i], "r");
	}
	*/

	// načtení dat a trénink

	// TODO pořešit cesty k datům
	FILE *vstup = fopen("../../data/fashion_mnist_train_vectors.csv", "r");
	FILE *vystupy = fopen("../../data/fashion_mnist_train_labels.csv", "r");
	pole_t vektory = nacist_data(vstup);
	float *priklady = vektory.data;
	int delka = vektory.velikost;
	int *cíle = nacist_cile(vystupy, delka);
	int správně = 0;
	for (int p = 0; p < DELKA_UCENI; ++p) {
		for (int i = 0; i < delka; ++i) {
			vysledky[0] = &priklady[i * 785];
			správně += trenink(vaha, neu, vysledky, derivace, vrstvy, pocty, cíle[i]);
		}
		printf("%d\n", správně);
	}
	free(priklady);
	return 0;
}

/* Přehled všeho, co ještě není dořešené:
1. Kde budou data relativně ke kódu
2. Nepoužívaný nultý pointer v neu a derivace
3. Vybrat správnou architekturu
4. Vybrat správnou aktivační funkci / více aktivačních funkcí?
5. Mají se upravit váhy po každém příkladu, nebo až po n-tici příkladů?
6. Konečně by se to mělo spustit
7. Vyřešit, jestli nebude rychlost učení na něčem závislá.


*/
