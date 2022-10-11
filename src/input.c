void bin_parsuj_vstupy(float *vstup, FILE *odkud, char **buff) {
  for (int i = 1; i < VSTUPU; ++i) {
    vstup[i - 1] = vstup[i];
  }
  unsigned int size = 20;
  getline(buff, &size, odkud);
  vstup[VSTUPU - 1] = atof(*buff);
  return;
}

void upload_koef(float **prvni_koef, float prvni_tr, float *druhy_koef,
                 float druhy_tr, FILE *kam) {
  for (int i = 0; i < NEU; ++i) {
    for (int j = 0; j < VSTUPU; ++j) {
      fprintf(kam, "%.4f\n", prvni_koef[j][i]);
    }
    fprintf(kam, "%.4f\n", prvni_tr[i]);
    fprintf(kam, "%.4f\n", druhy_koef[i]);
  }
  fprintf(kam, "%.4f\n", druhy_tr);
  return;
}

void import_koef(int vrstev, int *pocty, double ***vaha, FILE *odkud) {
  for (int i = 0; i < vrstev + 1; ++i) {
    for (int j = 0; j < pocty[i]; ++j) {
      for (int k = 0; k < pocty[i + 1]; ++k) {

        getline(buff, &size, odkud);
        prvni_koef[j][i] = atof(*buff);
      }
    }
    getline(buff, &size, odkud);
    prvni_tr[i] = atof(*buff);
    getline(buff, &size, odkud);
    druhy_koef[i] = atof(*buff);
  }
  getline(buff, &size, odkud);
  druhy_tr = atof(*buff);
  free(*buff);
  return;
}


char načíst[784 * 4];
FILE *vstup = fopen("../data/pv021_project/data/fashion_mnist_train_vectors.csv", "r");
for (int i = 0; i < 60000; ++i) {
	getline(načíst, )
}
trenink(vaha, neu, );

