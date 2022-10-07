#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define VSTUPU 30
#define NEU 10
#define SKLON 0.05
#define RYCHLOST 2

double sigmoida(double x) { return 1 / (1 + exp(-x)) }

double der_sigmoida(double x) { return sigmoida(x) * (1 - sigmoida(x)); }

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
  return SKLON;
}

double iterace(double ***vaha, double **vysledky, double **neu, int vrstvy,
               int *pocty) {
  for (int i = 0; i < vrstvy + 1; ++i) {
    vysledky[i][pocty[i]] = 1;
    for (int k = 0; k < pocty[i + 1]) {
      neu[i + 1][k] = 0;
      for (int j = 0; j < pocty[i] + 1; ++j) {
        neu[i + 1][k] += vysledky[i][j] * vaha[i][j][k];
      }
      vysledky[i + 1][k] = aktivacni(neu[i + 1][k]);
    }
  }
  return sigmoida(neu[vrstvy + 1][0]);
}

int trenink(double ***vaha, double **neu, double **vysledky, int vrstvy,
            int *pocty, double cil) {
  double vysledek = iterace(vaha, vysledky, neu, vrstvy, pocty);
  double *derivace[vrstvy + 1];
  double prvni[1];
  derivace[vrstvy] = prvni;
  derivace[vrstvy][0] = vysledek * (1 - vysledek);

  for (int i = vrstvy - 1; i >= 0, --i) {
    for (int j = 0; j < pocty[i]; ++j) {
      derivace[i][j] = 0;
      for (int k = 0; k < pocty[i + 1]; ++k) {
        derivace[i][j] +=
            vaha[i][j][k] * der_aktivacni(neu[i][j]) * derivace[i + 1][k];
      }
    }
  }
  for (int i = 0; i < vrstvy + 1) {
    for (int j = 0; j < pocty[i] + 1) {
      for (int k = 0; k < pocty[i + 1]) {
        vaha[i][j][k] -= (derivace[i][k] * vysledky[i][j] - cil) * RYCHLOST;
      }
    }
  }
  return vysledek - cil < 0.5;
}


int main(int argc, char **argv) {
  int vrstvy = argv[1];
  int pocty[vrstvy + 2];
  pocty[0] = VSTUPU;
  pocty[vrstvy + 1] = 1;

  for (int i = 1; i <= vrstvy; ++i) {
    pocty[i] = atoi(argv[i + 1]);
  }

  double **vaha[vrstvy + 1];

  for (j = 0; j < vrstvy + 1; ++j) {
    double vrstva[pocty[j]][pocty[j + 1]];
    vaha[j] = vrstva;
  }

  double *neu[vrstvy + 2];

  for (j = 0; j < vrstvy + 2; ++j) {
    double neurony[pocty[j]];
    memset(neurony, 0, pocty[j]);
    neu[j] = neurony;
  }

}
