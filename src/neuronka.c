#include "input.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define VSTUPU 784
#define VYSTUPU 10
#define SKLON 0.05
#define RYCHLOST 0.1
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
	//printf("iterace\n"); // debug
	for (int i = 0; i < vrstvy;
	     ++i) { // vrstev je vrstvy +2 (posledni index je vrstvy+1), ale posledni je výstupni
		// printf("%d. vrstva, %d neuronu v nizsi, %d neuronu ve vyssi\n", i, pocty[i], pocty[i + 1]); // debug
		vysledky[i][pocty[i]] = 1;                // nastaveni thresholdového neuronu
		for (int k = 0; k < pocty[i + 1]; ++k) {  // pro každý neuron vyšši vrstvy
			neu[i + 1][k] = 0;                  // vynulovani na začatku sumovani
			// printf("neuron %d vynulovan\n", k); // debug
			for (int j = 0; j < pocty[i] + 1;
			     ++j) { // pro každý neuron aktualni vrstvy +1 aby se počitalo i s thresholdem
				// printf("pricita se %d. neuron\n",j);
				// printf("spodni %f\n",vysledky[i][j]);
				// printf("%d, %d, %d\n",i,j,k);
				// printf("vaha %f\n",vaha[i][j][k]);
				neu[i + 1][k] += vysledky[i][j] * vaha[i][j][k];
			}
			//printf("suma %d neuronu %d vrstvy je: %f\n",k, i+1, neu[i + 1][k]);
			vysledky[i + 1][k] = relu(neu[i + 1][k]); // otazka vice aktivacnich funkci
		}
	}

	//řešeni výstupni vrstvy
	int max = 0;
	for (int i = 0; i < VYSTUPU; ++i) {
		neu[vrstvy + 1][i] = 0;
		vysledky[vrstvy][pocty[vrstvy]] = 1; // nastaveni thresholdu
		for (int j = 0; j < pocty[vrstvy] + 1; ++j) {
			neu[vrstvy + 1][i] += vysledky[vrstvy][j] * vaha[vrstvy][j][i];
		}
		//printf("suma %d neuronu %d vrstvy je: %f\n",i, vrstvy + 1, neu[vrstvy + 1][i]);
		if (neu[vrstvy + 1][i] > neu[vrstvy+1][max]) { //odecitani kvuli preteceni
		      max = i;
		}
	}
	float suma = 0;
	for (int i = 0; i < VYSTUPU; ++i) {
		suma += exp(neu[vrstvy + 1][i]- neu[vrstvy+1][max]); //odecitani kvuli preteceni
	}
	for (int i = 0; i < VYSTUPU; ++i) {
		vysledky[vrstvy + 1][i] = exp(neu[vrstvy + 1][i] - max) / suma; //odecitani kvuli preteceni
	}
	return max;
}

int trenink(float ***vaha, float **neu, float **vysledky, float **derivace, int vrstvy, int *pocty, int cil, int **odhady) {
	//printf("trenuju\n"); // debug
	int vysledek = iterace(vaha, vysledky, neu, vrstvy, pocty);
	//printf("%d %d\n",cil, vysledek); //debug
	odhady[cil][vysledek]+=1;
	//printf("hototvo\n"); //debug
	//printf("po iteraci, vysledek= %d\n", vysledek); // debug
	for (int i = 0; i < VYSTUPU; ++i) {
		if (i != cil) {
			derivace[vrstvy + 1][i] =
			      vysledky[vrstvy + 1]
			              [i]; // anebo += , připrava na přistup nekolik přikladů: jedno učeni
			//printf("derivace %d neuronu 3. vrstvy: %.2f\n", i, derivace[vrstvy + 1][i]); //debug
		}
	}
	derivace[vrstvy + 1][cil] = vysledky[vrstvy + 1][cil] - 1 ; // taky nebo +=
	//printf("derivace %d neuronu 3. vrstvy: %.2f\n", cil, derivace[vrstvy + 1][cil]); //debug

	for (int i = vrstvy; i > 0; --i) {
		for (int j = 0; j < pocty[i]; ++j) {
			derivace[i][j] = 0; // přesunout do nadřazené funkce ?
			for (int k = 0; k < pocty[i + 1]; ++k) {
				derivace[i][j] += vaha[i][j][k] * der_relu(neu[i][j]) * derivace[i + 1][k];
			}
			//printf("derivace %d neuronu %d vrstvy: %.4f\n", j,i, derivace[i][j]); //debug
		}
	}

	for (int i = 0; i < vrstvy + 1; ++i) {
		for (int j = 0; j < pocty[i] + 1; ++j) {
			for (int k = 0; k < pocty[i + 1]; ++k) {
				vaha[i][j][k] -= derivace[i + 1][k] * vysledky[i][j] *
				                 RYCHLOST; // RYCHLOST jako funkce nečeho ?
			}
		}
	}
	return vysledek == cil; // můžeme potom sečist výsledky tréninku pro celkový počet spravne
	                        // odhadnutých čislic
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

	printf("vahy\n"); // debug
	srand(0);

	for (int j = 0; j < vrstvy + 1; ++j) {
		vaha[j] = malloc((pocty[j] + 1) * sizeof(float *));
		for (int k = 0; k < pocty[j] + 1; ++k) {
			vaha[j][k] = malloc(pocty[j + 1] * sizeof(float));
			for (int g = 0; g < pocty[j + 1]; ++g) {
				vaha[j][k][g] = 0.3 * (float)rand() / (float)RAND_MAX - 0.15; //
				                   // inicializace vah mezi -1 a 1
				// printf("%f\n",vaha[j][k][g]); //debug
			}
		}
	}
	printf("%f\n", vaha[0][0][0]); // debug
	printf("neurony\n");           // debug
	float *neu[vrstvy + 2];        // neu[0] ale nepouživam (přehlednost ?)
	float *vysledky[vrstvy + 2];
	float *derivace[vrstvy + 2]; // derivace[0] taky nepouživam

	// neu[0]= float neurony[VSTUPU];
	// vysledky[0] = float vysledek[VSTUPU];

	for (int j = 1; j < vrstvy + 2; ++j) {
		neu[j] = malloc(pocty[j] *
		                sizeof(float)); // neu[j-1] ? neni potřeba sumovat vstupni vrstvu
		vysledky[j] = malloc((pocty[j] + 1) * sizeof(float));
		derivace[j] =
		      malloc(pocty[j] *
		             sizeof(float)); // nebo der[j-1] ? nepouzivam derivaci prvni vstupni vrstvy
	}
	printf("%f\n", derivace[1][2]); // debug

	/*
	if (argc > i) {
	  FILE *koef;
	  koef = fopen(argv[i], "r");
	}
	*/

	// načteni dat a trénink

	printf("hehe\n");
	// TODO pořešit cesty k datům
	FILE *vstup = fopen("../../data/fashion_mnist_train_vectors.csv", "r");
	printf("vstupy ok\n");
	FILE *vystupy = fopen("../../data/fashion_mnist_train_labels.csv", "r");
	printf("vystupy ok\n");
	int delka = secti_radky(vystupy);
	float *priklady = nacist_data(vstup, delka);
	printf("nacitam cile\n");
	int *cile = nacist_cile(vystupy, delka);
	int spravne;
	int *odhady[10];
	
	

	/* // debug
	for (int i=59999*785+1; i< 60000*785; ++i){
	printf("%f, ",priklady[i]);
	}
	printf("\n");
	printf("%d\n",cile[59999]);
	*/
	FILE *ven=fopen("vahy2.txt","w");
	printf("zacatek treninku\n");
	for (int p = 0; p < DELKA_UCENI; ++p) {
		for (int i=0;i<10;++i){
			odhady[i]=malloc(10*sizeof(int));
			for (int j=0;j<10;++j){
				odhady[i][j]=0;
			}
		}
		//printf("%d. kolo\n", p); //debug
		spravne=0;
		for (int i = 0; i < delka; ++i) {
			vysledky[0] = &priklady[i * 785];
			/* //debug
			for (int j=0; j<784;++j){
			      printf("%.0f, ",vysledky[0][j]);
			}
			printf("\n");
			*/
			//vypis_vahy(ven, vaha,pocty, vrstvy); //debug
			spravne += trenink(vaha, neu, vysledky, derivace, vrstvy, pocty, cile[i], odhady);
			
			//fprintf(ven,"=====================================================\n");
		}
		for (int v=0; v<10;++v){
			for (int w=0; w<10;++w){
				printf("%8d",odhady[v][w]);
			}
			printf("\n");
		}
		vypis_vahy(ven, vaha,pocty, vrstvy); //debug
		printf("%d. kolo %d\n", p,spravne);
	}
	// vypis_vahy(ven, vaha,pocty, vrstvy);
	free(priklady);
	return 0;
}

/* Přehled všeho, co ješte neni dořešené:
1. Kde budou data relativne ke kódu
2. Nepouživaný nultý pointer v neu a derivace
3. Vybrat spravnou architekturu
4. Vybrat spravnou aktivačni funkci / vice aktivačnich funkci?
5. Maji se upravit vahy po každém přikladu, nebo až po n-tici přikladů?
6. Konečne by se to melo spustit
7. Vyřešit, jestli nebude rychlost učeni na nečem zavisla.


*/
