#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define VSTUPU 784
#define VYSTUPU 10
#define NEU 10
#define SKLON 0.05
#define RYCHLOST 2
#define DELKA_UCENI 5000

// za sebe mluvící různé pokusné aktivační funkce

double sigmoida(double x) { return 1 / (1 + exp(-x)); }

double der_sigmoida(double x) { return sigmoida(x) * (1 - sigmoida(x)); }

double relu(double x) {
	if (x > 0) {
		return x;
	}
	return 0; // SKLON * x;
}

double der_relu(double x) {
	if (x > 0) {
		return 1;
	}
	return SKLON; // 0;
}

double logaritmická(double x) {
	if (x > 0) {
		return log(x + 1);
	}
	return 0; // SKLON * x;
}

double der_logaritmická(double x) {
	if (x > 0) {
		return 1 / (x + 1);
	}
	return SKLON; // 0;
}

double kvadrát(double x) {
	if (x > -1 && x < 1) {
		return x * x - 1;
	}
	return 0;
}

double der_kvadrát(double x) {
	if (x < -1) {
		return SKLON; // 0;
	}
	if (x > 1) {
		return -SKLON; // 0;
	}
	return x;
}

/*
iterace provede jeden průchod neuronovou sítí a vrátí číslo uhodnuté kategotie
double ***vaha - vaha[i][j][k] váha spoje mezi neuronem ve vrstvě i indexu j a neuronem ve vrstvě
i+1 a indexem k double **vysledky vysledky[0] je 784 vstupů +1 threshold, jinak není třeba
inicializace double **neu neinicializované pole pro všechny neurony mimo thresholdy int vrstvy počet
skrytých vrstev int *pocty počet neuronů v každé vrstvě bez thresholdů
*/

int iterace(double ***vaha, double **vysledky, double **neu, int vrstvy, int *pocty) {
	for (int i = 0; i < vrstvy + 1; ++i) { // vrstev je vrstvy +2, ale poslední je výstupní
		vysledky[i][pocty[i]] = 1;       // nastavení thresholdového neuronu
		for (int k = 0; k < pocty[i + 1]; ++k) {       // pro každý neuron vyšší vrstvy
			neu[i + 1][k] = 0;                       // vynulování na začátku sumování
			for (int j = 0; j < pocty[i] + 1; ++j) { // +1 aby se počítalo i s thresholdem
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
	double suma = 0;
	for (int i = 0; i < VYSTUPU; ++i) {
		suma += exp(neu[vrstvy + 1][i]); //- neu[vrstvy+1][max]);
	}
	for (int i = 0; i < VYSTUPU; ++i) {
		vysledky[vrstvy + 1][i] = exp(neu[vrstvy + 1][i]) / suma; // - max) / suma;
	}

	return max;
}

int trenink(double ***vaha, double **neu, double **vysledky, double **derivace, int vrstvy,
            int *pocty, int cil) {
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
				vaha[i][j][k] -= derivace[i][k] * vysledky[i][j] *
				                 RYCHLOST; // RYCHLOST jako funkce něčeho ?
			}
		}
	}
	return vysledek == cil; // můžeme potom sečíst výsledky tréninku pro celkový počet správně
	                        // odhadnutých číslic
}

int main(int argc, char **argv) {
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

	double **vaha[vrstvy + 1];
	srand(0);
	for (int j = 0; j < vrstvy + 1; ++j) {
		double vrstva[pocty[j]][pocty[j + 1]];
		for (int k = 0; k < pocty[j] + 1; ++k) {
			for (int g = 0; g < pocty[j + 1]; ++g) {
				vrstva[k][g] = 2 * (double)rand() / (double)RAND_MAX -
				               1; // inicializace vah mezi -1 a 1
			}
		}
		vaha[j] = vrstva;
	}

	double *neu[vrstvy + 2]; // neu[0] ale nepoužívám (přehlednost ?)
	double *vysledky[vrstvy + 2];
	double *derivace[vrstvy + 2]; // derivace[0] taky nepoužívám

	// neu[0]= double neurony[VSTUPU];
	// vysledky[0] = double vysledek[VSTUPU];

	for (int j = 1; j < vrstvy + 2; ++j) {
		double neurony[pocty[j]];
		double vysledek[pocty[j] + 1];
		double der[pocty[j]];
		// memset(neurony, 0, pocty[j]); //není třeba
		neu[j] = neurony; // neu[j-1] není potřeba sumovat vstupní vrstvu
		vysledky[j] = vysledek;
		derivace[j] = der; // nebo der[j-1] nepouzivam derivaci prvni vstupni vrstvy
	}

	/*
	if (argc > i) {
	  FILE *koef;
	  koef = fopen(argv[i], "r");
	}
	*/

	// TODO pořešit cesty k datům
	FILE *vstup = fopen("../data/pv021_project/data/fashion_mnist_train_vectors.csv", "r");
	FILE *výstupy = fopen("../data/pv021_project/data/fashion_mnist_train_labels.csv", "r");
	double **data = načíst_data(vstup);
	int *cíle = načíst_cíle(výstupy);
	int správně = 0;
	for (int p = 0; p < DELKA_UCENI; ++p) {
		for (int i = 0; i < 60000; ++i) {
			vysledky[0] = data[i];
			správně += trenink(vaha, neu, vysledky, derivace, vrstvy, pocty, cíle[i]);
		}
		printf("%d\n", správně);
	}
	return 0;
}
