#ifndef PV021_PROJEKT_INPUT_H
#define PV021_PROJEKT_INPUT_H

#include <stddef.h>
#include <stdio.h>

typedef struct {
	float *data;
	size_t velikost;
} pole_t;

/**
 * Velikost bufferu pro načitání souboru
 */
#define VELIKOST_BUFFERU (1 << 16)
#define VELIKOST_OBRAZKU (28 * 28)

/**
 * Toto bere deskriptor souboru se vstupními daty a vrací pole vstupních obrazku.
 * Každý vstupní obrazek je pole floatu.
 *
 * @param soubor ukazatel na soubor budoucí cteným
 * @return pole zpracovaných vstupních dat
 */
float *nacist_data(FILE *soubor, int pocet_radku);

int *nacist_cile(FILE *soubor, int pocet_radku);

/**
 * Toto precte daný soubor a vrati pocet radku v něm.
 * Toto vrati ukazatel pozice v tom souboru na zacatek.
 *
 * @param soubor
 * @return pocet rAdku
 */
int secti_radky(FILE *soubor);

void vypis_vahy(FILE *ven, float ***vaha, int *pocty, int vrstvy);

#endif // PV021_PROJEKT_INPUT_H
