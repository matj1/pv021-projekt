#include "input.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define VSTUPU 784
#define VYSTUPU 10
#define SKLON 0.005
#define RYCHLOST 0.01 //když je moc velká, tak celá neuronka spadne do předpovídání jediné kategorie
#define DELKA_UCENI 5000
#define BEC 10


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

int iterace(float ***vaaha, float **vyysledky, float **neu, int vrstvy, int *pocsty) {
	for (int i = 0; i < vrstvy;
	     ++i) { // vrstev je vrstvy +2 (posledni index je vrstvy+1), ale posledni je výstupni
		vyysledky[i][pocsty[i]] = 1;                // nastaveni thresholdového neuronu
		for (int k = 0; k < pocsty[i + 1]; ++k) {  // pro každý neuron vyšši vrstvy
			neu[i + 1][k] = 0;                  // vynulovani na začatku sumovani
			for (int j = 0; j < pocsty[i] + 1;
			     ++j) { // pro každý neuron aktualni vrstvy +1 aby se počitalo i s thresholdem
				neu[i + 1][k] += vyysledky[i][j] * vaaha[i][j][k];
			}
			vyysledky[i + 1][k] = relu(neu[i + 1][k]); // otazka vice aktivacnich funkci
		}
	}

	//řešeni výstupni vrstvy
	int max = 7;
	neu[vrstvy + 1][7] = 0;
	vyysledky[vrstvy][pocsty[vrstvy]] = 1; // nastaveni thresholdu
	for (int i = 0; i < VYSTUPU; ++i) {
		neu[vrstvy + 1][i] = 0;
		for (int j = 0; j < pocsty[vrstvy] + 1; ++j) {
			neu[vrstvy + 1][i] += vyysledky[vrstvy][j] * vaaha[vrstvy][j][i];
		}
		if (neu[vrstvy + 1][i] > neu[vrstvy+1][max]) { //odecitani kvuli preteceni
			max = i;
		}
	}

	float suma = 0;
	for (int i = 0; i < VYSTUPU; ++i) {
		suma += exp(neu[vrstvy + 1][i]- neu[vrstvy+1][max]); //odecitani kvuli preteceni
	}
	for (int i = 0; i < VYSTUPU; ++i) {
		vyysledky[vrstvy + 1][i] = exp(neu[vrstvy + 1][i] - neu[vrstvy + 1][max]) / suma; //odecitani kvuli preteceni
	}
	return max;
}

int bekpropagejsn(float ***vaaha, float **neu, float **vyysledky, float **derivace, int vrstvy, int *pocsty, int ciil, int **odhady) {
	int vyysledek = iterace(vaaha, vyysledky, neu, vrstvy, pocsty);
	odhady[ciil][vyysledek]+=1;
	for (int i = 0; i < VYSTUPU; ++i) {
		if (i != ciil) {
			derivace[vrstvy + 1][i] = vyysledky[vrstvy + 1][i];
		}
	}
	derivace[vrstvy + 1][ciil] = vyysledky[vrstvy + 1][ciil] - 1;
	for (int i = vrstvy; i > 0; --i) {
		for (int j = 0; j < pocsty[i]; ++j) {
			derivace[i][j] = 0;
			for (int k = 0; k < pocsty[i + 1]; ++k) {
				derivace[i][j] += vaaha[i][j][k] * der_relu(neu[i][j]) * derivace[i + 1][k];
			}
		}
	}
	return vyysledek == ciil; // můžeme potom sečist výsledky tréninku pro celkový počet spravne odhadnutých čislic
}

int trenink(float *prziikady, int pozice, float ***vaaha, float **neu, float ***vyysledky, float ***derivace, int vrstvy, int *pocsty, int *ciile, int **odhady) {
	int spraavnje=0;

	//TODO tohle je tvoje parketa na paralelizaci, už se na ni těším

	for (int i=0; i<BEC; ++i){
		vyysledky[i][0] = &prziikady[(pozice + i) * (VSTUPU+1)];
		spraavnje+=bekpropagejsn(vaaha, neu, vyysledky[i], derivace[i], vrstvy, pocsty, ciile[pozice+i], odhady);
	}
	//změna vah
	for (int i=0; i< BEC; ++i){
		//printf("%d beč\n",i);
		for (int j = 0; j < vrstvy+1 ; ++j) {
			for (int l = 0; l < pocsty[j + 1]; ++l) {
				for (int k = 0; k < pocsty[j] + 1; ++k) {
					vaaha[j][k][l] -= derivace[i][j + 1][l] * vyysledky[i][j][k] * RYCHLOST; // RYCHLOST jako funkce nečeho ?
				}
			}
		}
	}
	return spraavnje;
}

int main(int argc, char **argv) {

	// deklarace všech velkých poli

	int vrstvy = atoi(argv[1]); // uzivatel zada pocet skrytych (ne vstupnich, ne
	                            // vystupnich) vrstev
	int pocsty[vrstvy + 2];
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
	float **vaaha[vrstvy + 1];
	srand(2);
	for (int j = 0; j < vrstvy + 1; ++j) {
		vaaha[j] = malloc((pocsty[j] + 1) * sizeof(float *));
		for (int k = 0; k < pocsty[j] + 1; ++k) {
			vaaha[j][k] = malloc(pocsty[j + 1] * sizeof(float));
			for (int g = 0; g < pocsty[j + 1]; ++g) {
				vaaha[j][k][g] = 0.1 * (float)rand() / (float)RAND_MAX - 0.05; //
				                   // inicializace vah
			}
		}
	}
	float *neu[vrstvy + 2];        // neu[0] ale nepouživam (přehlednost ?)
	float **vyysledky[BEC];
	float **derivace[BEC];
	for (int j = 1; j < vrstvy + 2; ++j) {
		neu[j] = malloc(pocsty[j] *
		                sizeof(float)); // neu[j-1] ? neni potřeba sumovat vstupni vrstvu
	}
	for (int k=0; k<BEC; ++k){
		vyysledky[k]=malloc((vrstvy+2)*sizeof(float *));
		for (int i=0; i<vrstvy+2; ++i){
			vyysledky[k][i] = malloc((pocsty[i] + 1) * sizeof(float)); //vim, ze neni potreba alokovat threshold vystupni vrstvy, ale co uz
		}
	}

	for (int k=0; k<BEC; ++k){
		derivace[k]=malloc((vrstvy+2)*sizeof(float *));
		for (int i=1; i<vrstvy+2; ++i){ //zmena i=0
			derivace[k][i] = malloc(pocsty[i] * sizeof(float));
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

	float *prziikady = nacist_data(vstup, deelka);
	float *val_prziikady = nacist_data(validacni_vektor, val_deelka);

	printf("nacitam cile\n");
	int *ciile = nacist_cile(vyystupy, deelka);
	int *val_ciile = nacist_cile(validacni_popisek, val_deelka);
	
	int spraavnje;
	int *odhady[10]; //confusion matrix

	FILE *ven=fopen("vahy2.txt","w");
	printf("zacatek treninku\n");
	for (int i=0;i<10;++i){
		odhady[i]=malloc(10*sizeof(int));
	}

	for (int p = 0; p < 50; ++p) {
		for (int i=0;i<10;++i){
			memset(odhady[i],0,10*sizeof(int));
		}

		spraavnje=0;
		for (int i = 0; i < deelka -BEC +1; i+=BEC) {
			spraavnje += trenink(prziikady, i, vaaha, neu, vyysledky, derivace, vrstvy, pocsty, ciile, odhady);
		}

		for (int v=0; v<10;++v){ //confusion matrix
			for (int w=0; w<10;++w){
				printf("%8d",odhady[v][w]);
			}
			printf("\n");
		}
		printf("%d. kolo %d\n", p,spraavnje);

		printf("validace\n");
		spraavnje=0;

		for (int i=0;i<10;++i){
			memset(odhady[i],0,10*sizeof(int));
		}
		int vyysledek;
		for (int i=0; i< val_deelka;++i){
			vyysledky[0][0]=&val_prziikady[i*(VSTUPU+1)];
			vyysledek=iterace(vaaha, vyysledky[0], neu, vrstvy, pocsty);
			spraavnje+=val_ciile[i]==vyysledek;
			odhady[val_ciile[i]][vyysledek]+=1;
		}

		for (int v=0; v<10;++v){ //confusion matrix
			for (int w=0; w<10;++w){
				printf("%8d",odhady[v][w]);
			}
			printf("\n");
		}
		printf("validace po %d. kole %d\n", p,spraavnje);
	}
	vypis_vahy(ven, vaaha,pocsty, vrstvy);
	free(prziikady);
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
