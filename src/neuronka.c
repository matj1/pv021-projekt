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

float ***vaha - vaha[i][j][k] vaha spoje mezi neuronem ve vrstve i indexu j a neuronem ve vrstve
i+1 a indexem k

float **vysledky vysledky[0] je 784 vstupů +1 threshold, jinak neni třeba
inicializace

float **neu neinicializované pole pro všechny neurony mimo thresholdy

int vrstvy počet skrytých vrstev

int *pocty počet neuronů v každé vrstve bez thresholdů
*/

int iterace(float ***vaha, float **vysledky, float **neu, int vrstvy, int *pocty) {
	for (int i = 0; i < vrstvy;
	     ++i) { // vrstev je vrstvy +2 (posledni index je vrstvy+1), ale posledni je výstupni
		vysledky[i][pocty[i]] = 1;                // nastaveni thresholdového neuronu
		for (int k = 0; k < pocty[i + 1]; ++k) {  // pro každý neuron vyšši vrstvy
			neu[i + 1][k] = 0;                  // vynulovani na začatku sumovani
			for (int j = 0; j < pocty[i] + 1;
			     ++j) { // pro každý neuron aktualni vrstvy +1 aby se počitalo i s thresholdem
				neu[i + 1][k] += vysledky[i][j] * vaha[i][j][k];
			}
			vysledky[i + 1][k] = relu(neu[i + 1][k]); // otazka vice aktivacnich funkci
		}
	}

	//řešeni výstupni vrstvy
	int max = 7;
	neu[vrstvy + 1][7] = 0;
	vysledky[vrstvy][pocty[vrstvy]] = 1; // nastaveni thresholdu
	for (int i = 0; i < VYSTUPU; ++i) {
		neu[vrstvy + 1][i] = 0;
		for (int j = 0; j < pocty[vrstvy] + 1; ++j) {
			neu[vrstvy + 1][i] += vysledky[vrstvy][j] * vaha[vrstvy][j][i];
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
		vysledky[vrstvy + 1][i] = exp(neu[vrstvy + 1][i] - neu[vrstvy + 1][max]) / suma; //odecitani kvuli preteceni
	}
	return max;
}

int bekpropagejsn(float ***vaha, float **neu, float **vysledky, float **derivace, int vrstvy, int *pocty, int cil, int **odhady) {
	int vysledek = iterace(vaha, vysledky, neu, vrstvy, pocty);
	odhady[cil][vysledek]+=1;
	for (int i = 0; i < VYSTUPU; ++i) {
		if (i != cil) {
			derivace[vrstvy + 1][i] = vysledky[vrstvy + 1][i];
		}
	}
	derivace[vrstvy + 1][cil] = vysledky[vrstvy + 1][cil] - 1;
	for (int i = vrstvy; i > 0; --i) {
		for (int j = 0; j < pocty[i]; ++j) {
			derivace[i][j] = 0;
			for (int k = 0; k < pocty[i + 1]; ++k) {
				derivace[i][j] += vaha[i][j][k] * der_relu(neu[i][j]) * derivace[i + 1][k];
			}
		}
	}
	return vysledek == cil; // můžeme potom sečist výsledky tréninku pro celkový počet spravne odhadnutých čislic
}

int trenink(float *priklady, int pozice, float ***vaha, float **neu, float ***vysledky, float ***derivace, int vrstvy, int *pocty, int *cile, int **odhady) {
	int spravne=0;

	//TODO tohle je tvoje parketa na paralelizaci, už se na ni těším

	for (int i=0; i<BEC; ++i){
		vysledky[i][0] = &priklady[(pozice + i) * (VSTUPU+1)];
		spravne+=bekpropagejsn(vaha, neu, vysledky[i], derivace[i], vrstvy, pocty, cile[pozice+i], odhady);
	}
	//změna vah
	for (int i=0; i< BEC; ++i){
		//printf("%d beč\n",i);
		for (int j = 0; j < vrstvy+1 ; ++j) {
			for (int l = 0; l < pocty[j + 1]; ++l) {
				for (int k = 0; k < pocty[j] + 1; ++k) {
					vaha[j][k][l] -= derivace[i][j + 1][l] * vysledky[i][j][k] * RYCHLOST; // RYCHLOST jako funkce nečeho ?
				}
			}
		}
	}
	return spravne;
}

int main(int argc, char **argv) {

	// deklarace všech velkých poli

	int vrstvy = atoi(argv[1]); // uzivatel zada pocet skrytych (ne vstupnich, ne
	                            // vystupnich) vrstev
	int pocty[vrstvy + 2];
	pocty[0] = VSTUPU;
	pocty[vrstvy + 1] = VYSTUPU;
	printf("%d\n", pocty[0]);
	for (int i = 1; i <= vrstvy; ++i) {
		pocty[i] = atoi(argv[i + 1]); // uzivatel zada pocty skrytych neuronu jako
		                              // dalsi argumenty main (tolik cisel, kolik
		                              // deklaroval, ze bude zadavat)
		printf("%d\n", pocty[i]);
	}
	printf("%d\n", pocty[vrstvy + 1]);
	float **vaha[vrstvy + 1];
	srand(2);
	for (int j = 0; j < vrstvy + 1; ++j) {
		vaha[j] = malloc((pocty[j] + 1) * sizeof(float *));
		for (int k = 0; k < pocty[j] + 1; ++k) {
			vaha[j][k] = malloc(pocty[j + 1] * sizeof(float));
			for (int g = 0; g < pocty[j + 1]; ++g) {
				vaha[j][k][g] = 0.1 * (float)rand() / (float)RAND_MAX - 0.05; //
				                   // inicializace vah
			}
		}
	}
	float *neu[vrstvy + 2];        // neu[0] ale nepouživam (přehlednost ?)
	float **vysledky[BEC];
	float **derivace[BEC];
	for (int j = 1; j < vrstvy + 2; ++j) {
		neu[j] = malloc(pocty[j] *
		                sizeof(float)); // neu[j-1] ? neni potřeba sumovat vstupni vrstvu
	}
	for (int k=0; k<BEC; ++k){
		vysledky[k]=malloc((vrstvy+2)*sizeof(float *));
		for (int i=0; i<vrstvy+2; ++i){
			vysledky[k][i] = malloc((pocty[i] + 1) * sizeof(float)); //vim, ze neni potreba alokovat threshold vystupni vrstvy, ale co uz
		}
	}

	for (int k=0; k<BEC; ++k){
		derivace[k]=malloc((vrstvy+2)*sizeof(float *));
		for (int i=0; i<vrstvy+2; ++i){
			derivace[k][i] = malloc(pocty[i] * sizeof(float));
		}
	}
	// načteni dat a trénink
	// TODO pořešit cesty k datům
	FILE *vstup = fopen("../../data/train_vectors.csv", "r");
	FILE *validacni_vektor = fopen("../../data/val_train_vectors.csv", "r");
	
	printf("vstupy ok\n");
	FILE *vystupy = fopen("../../data/train_labels.csv", "r");
	FILE *validacni_popisek = fopen("../../data/val_train_labels.csv", "r");

	printf("vystupy ok\n");
	int delka = secti_radky(vystupy);
	int val_delka = secti_radky(validacni_popisek);

	float *priklady = nacist_data(vstup, delka);
	float *val_priklady = nacist_data(validacni_vektor, val_delka);

	printf("nacitam cile\n");
	int *cile = nacist_cile(vystupy, delka);
	int *val_cile = nacist_cile(validacni_popisek, val_delka);
	
	int spravne;
	int *odhady[10];

	FILE *ven=fopen("vahy2.txt","w");
	printf("zacatek treninku\n");
	for (int i=0;i<10;++i){
		odhady[i]=malloc(10*sizeof(int));
		for (int j=0;j<10;++j){
			odhady[i][j]=0;
		}
	}

	for (int p = 0; p < 50; ++p) {
		for (int i=0;i<10;++i){
			memset(odhady[i],0,10*sizeof(int));
		}

		spravne=0;
		for (int i = 0; i < delka -BEC +1; i+=BEC) {
			spravne += trenink(priklady, i, vaha, neu, vysledky, derivace, vrstvy, pocty, cile, odhady);
		}

		for (int v=0; v<10;++v){
			for (int w=0; w<10;++w){
				printf("%8d",odhady[v][w]);
			}
			printf("\n");
		}
		printf("%d. kolo %d\n", p,spravne);

		printf("validace\n");
		spravne=0;

		for (int i=0;i<10;++i){
			memset(odhady[i],0,10*sizeof(int));
		}
		int vysledek;
		for (int i=0; i< val_delka;++i){
			vysledky[0][0]=&val_priklady[i*(VSTUPU+1)];
			vysledek=iterace(vaha, vysledky[0], neu, vrstvy, pocty);
			spravne+=val_cile[i]==vysledek;
			odhady[val_cile[i]][vysledek]+=1;
		}

		for (int v=0; v<10;++v){
			for (int w=0; w<10;++w){
				printf("%8d",odhady[v][w]);
			}
			printf("\n");
		}
		printf("validace po %d. kole %d\n", p,spravne);
	}
	vypis_vahy(ven, vaha,pocty, vrstvy);
	free(priklady);
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
