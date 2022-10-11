#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define VSTUPŮ 30
#define NEU 10
#define SKLON 0.05
#define RYCHLOST 2

double sigmoida(double x) { return 1 / (1 + exp(-x)); }

double der_sigmoida(double x) { return sigmoida(x) * (1 - sigmoida(x)); }

double aktivační(double x) {
	if (x > 0) {
		return log(x + 1);
	}
	return 0; // SKLON * x;
}

double der_aktivační(double x) {
	if (x > 0) {
		return 1 / (x + 1);
	}
	return SKLON;
}

double iterace(double ***váha, double **výsledky, double **neu, int vrstvy, int *počty) {
	for (int i = 0; i < vrstvy + 1; ++i) {
		výsledky[i][počty[i]] = 1;
		for (int k = 0; k < počty[i + 1]; k++) {
			neu[i + 1][k] = 0;
			for (int j = 0; j < počty[i] + 1; ++j) {
				neu[i + 1][k] += výsledky[i][j] * váha[i][j][k];
			}
			výsledky[i + 1][k] = aktivační(neu[i + 1][k]);
		}
	}
	return sigmoida(neu[vrstvy + 1][0]);
}

int tréning(
      double ***váha,
      double **neu,
      double **výsledky,
      int vrstvy,
      int *počty,
      double cíl
) {
	double vysledek = iterace(váha, výsledky, neu, vrstvy, počty);
	double *derivace[vrstvy + 1];
	double prvni[1];
	derivace[vrstvy] = prvni;
	derivace[vrstvy][0] = vysledek * (1 - vysledek);

	for (int i = vrstvy - 1; i >= 0; i--) {
		for (int j = 0; j < počty[i]; j++) {
			derivace[i][j] = 0;
			for (int k = 0; k < počty[i + 1]; ++k) {
				derivace[i][j] +=
				      váha[i][j][k] * der_aktivační(neu[i][j]) * derivace[i + 1][k];
			}
		}
	}
	for (int i = 0; i < vrstvy + 1; i++) {
		for (int j = 0; j < počty[i] + 1; j++) {
			for (int k = 0; k < počty[i + 1]; k++) {
				váha[i][j][k] -= (derivace[i][k] * výsledky[i][j] - cíl) * RYCHLOST;
			}
		}
	}
	return vysledek - cíl < 0.5;
}

int main(int argc, char **argv) {
	int vrstvy = atoi(argv[1]);
	int pocty[vrstvy + 2];
	pocty[0] = VSTUPŮ;
	pocty[vrstvy + 1] = 1;

	for (int i = 1; i <= vrstvy; ++i) {
		pocty[i] = atoi(argv[i + 1]);
	}

	double **váha[vrstvy + 1];

	for (int j = 0; j < vrstvy + 1; ++j) {
		double vrstva[pocty[j]][pocty[j + 1]];
		váha[j] = vrstva;
	}

	double *neu[vrstvy + 2];

	for (int j = 0; j < vrstvy + 2; ++j) {
		double neurony[pocty[j]];
		memset(neurony, 0, pocty[j]);
		neu[j] = neurony;
	}
}
