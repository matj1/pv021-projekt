#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define VSTUPU 784
#define VYSTUPU 10
#define NEU 10
#define SKLON 0.05
#define RYCHLOST 2

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

double aktivacni(double x) {
	if (x > 0) {
		return log(x + 1);
	}
	return 0; // SKLON * x;
}

double der_aktivacni(double x) {
	if (x > 0) {
		return 1 / (x + 1);
	}
	return SKLON; // 0;
}

double aktivacni_kv(double x) {
	if (x > -1 && x < 1) {
		return x * x - 1;
	}
	return 0;
}

double der_aktivacni_kv(double x) {
	if (x < -1) {
		return SKLON; // 0;
	}
	if (x > 1) {
		return -SKLON; // 0;
	}
	return x
}

double iterace(double ***vaha, double **vysledky, double **neu, int vrstvy, int *pocty) {
	for (int i = 0; i < vrstvy; ++i) {
		vysledky[i][pocty[i]] = 1;
		for (int k = 0; k < pocty[i + 1]) {
			neu[i + 1][k] = 0;
			for (int j = 0; j < pocty[i] + 1; ++j) {
				neu[i + 1][k] += vysledky[i][j] * vaha[i][j][k];
			}
			vysledky[i + 1][k] = aktivacni(neu[i + 1][k]); // otazka vice aktivacnich funkci
		}
	}
	double max = 0;
	for (int i = 0; i < VYSTUPU; ++i) {
		neu[vrstvy + 1][i] = 0;
		for (int j = 0; j < pocty[vrstvy]; ++j) {
			neu[vrstvy + 1][i] += vysledky[vrstvy][j] * vaha[vrstvy][j][i];
		}
		if (neu[vrstvy + 1][i] > max) {
			max = neu[vrstvy + 1][i];
		}
	}
	double suma = 0;
	for (int i = 0; i < VYSTUPU; ++i) {
		suma += exp(neu[vrstvy + 1][i] - max);
	}
	for (int i = 0; i < VYSTUPU; ++i) {
		vysledky[vrstvy + 1][i] = exp(neu[vrstvy + 1][i] - max) / suma;
	}

	return 0;
}

int trenink(double ***vaha, double **neu, double **vysledky, double **derivace, int vrstvy,
            int *pocty, int cil) {
	iterace(vaha, vysledky, neu, vrstvy, pocty);

	for (int i = 0; i < VYSTUPU; ++i) {
		if (i != cil) {
			derivace[vrstvy + 1][i] = vysledky[vrstvy + 1][i]; // anebo +=
		}
	}
	derivace[vrstvy + 1][cil] = 1 - vysledky[vrstvy + 1][cil]; // taky nebo +=

	for (int i = vrstvy; i > 0, --i) {
		for (int j = 0; j < pocty[i]; ++j) {
			derivace[i][j] = 0; // presunout do main ?
			for (int k = 0; k < pocty[i + 1]; ++k) {
				derivace[i][j] +=
				      vaha[i][j][k] * der_aktivacni(neu[i][j]) * derivace[i + 1][k];
			}
		}
	}

	for (int i = 0; i < vrstvy + 1) { // +2 ?
		for (int j = 0; j < pocty[i] + 1) {
			for (int k = 0; k < pocty[i + 1]) {
				vaha[i][j][k] -= (derivace[i][k] * vysledky[i][j] - cil) * RYCHLOST;
			}
		}
	}
	return 0; // vysledek - cil < 0.5;
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

	for (int j = 0; j < vrstvy + 1; ++j) {
		double vrstva[pocty[j]][pocty[j + 1]];
		vaha[j] = vrstva;
	}

	double *neu[vrstvy + 2];
	double *vysledky[vrstvy + 2];
	double *derivace[vrstvy + 2];

	// neu[0]= double neurony[VSTUPU];
	vysledky[0] = double vysledek[VSTUPU];

	for (int j = 1; j < vrstvy + 2; ++j) {
		double neurony[pocty[j]];
		double vysledek[pocty[j]];
		double der[pocty[j]];
		// memset(neurony, 0, pocty[j]);
		neu[j] = neurony;
		vysledky[j] = vysledek;
		derivace[j] = der; // nebo der[j-1] nepouzivam derivaci prvni vstupni vrstvy
	}

	/*
	if (argc > i) {
	  FILE *koef;
	  koef = fopen(argv[i], "r");
	}
	*/

	trenink(vaha, neu, );
	return 0;
}