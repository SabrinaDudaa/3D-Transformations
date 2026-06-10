#include "algebra.h"
#include <stdlib.h>
#include <string.h>

// Aloca uma matriz 4x4 de floats
static float **alocaMatriz4d() {
    float **m = (float **)malloc(4 * sizeof(float *));
    for (int i = 0; i < 4; i++)
        m[i] = (float *)calloc(4, sizeof(float));
    return m;
}

void criaIdentidade4d(float **m) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m[i][j] = (i == j) ? 1.0f : 0.0f;
}

void imprimeMatriz4dDBG(float **m) {
    for (int i = 0; i < 4; i++) {
        printf("[ ");
        for (int j = 0; j < 4; j++)
            printf("%8.4f ", m[i][j]);
        printf("]\n");
    }
}

// Retorna novo vetor alocado com 4 floats: resultado = matriz * ponto
float *multMatriz4dPonto(float **m, float *p) {
    float *res = (float *)calloc(4, sizeof(float));
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            res[i] += m[i][j] * p[j];
    return res;
}

// matrizB = matrizA * matrizB  (resultado sobrescreve matrizB)
void multMatriz4d(float **matrizA, float **matrizB) {
    float **tmp = alocaMatriz4d();

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            tmp[i][j] = 0.0f;
            for (int k = 0; k < 4; k++)
                tmp[i][j] += matrizA[i][k] * matrizB[k][j];
        }

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            matrizB[i][j] = tmp[i][j];

    for (int i = 0; i < 4; i++) free(tmp[i]);
    free(tmp);
}