#include "objeto.h"
#include "algebra.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

// -----------------------------------------------------------------------
// Helpers internos
// -----------------------------------------------------------------------

static float **alocaMatriz4d() {
    float **m = (float **)malloc(4 * sizeof(float *));
    for (int i = 0; i < 4; i++)
        m[i] = (float *)calloc(4, sizeof(float));
    return m;
}

static void liberaMatriz4d(float **m) {
    for (int i = 0; i < 4; i++) free(m[i]);
    free(m);
}

// -----------------------------------------------------------------------
// carregaObjeto
// -----------------------------------------------------------------------
tObjeto3d *carregaObjeto(char *nomeArquivo) {
    FILE *f = fopen(nomeArquivo, "r");
    if (!f) {
        printf("Erro ao abrir arquivo: %s\n", nomeArquivo);
        return NULL;
    }

    tObjeto3d *obj = (tObjeto3d *)malloc(sizeof(tObjeto3d));

    // --- vértices ---
    fscanf(f, "%d", &obj->nPontos);
    obj->pontos = (float **)malloc(obj->nPontos * sizeof(float *));
    for (int i = 0; i < obj->nPontos; i++) {
        obj->pontos[i] = (float *)malloc(4 * sizeof(float)); // x y z w
        fscanf(f, "%f %f %f", &obj->pontos[i][0], &obj->pontos[i][1], &obj->pontos[i][2]);
        obj->pontos[i][3] = 1.0f; // coordenada homogênea
    }

    // --- arestas ---
    fscanf(f, "%d", &obj->nArestas);
    obj->arestas = (int **)malloc(obj->nArestas * sizeof(int *));
    for (int i = 0; i < obj->nArestas; i++) {
        obj->arestas[i] = (int *)malloc(2 * sizeof(int));
        fscanf(f, "%d %d", &obj->arestas[i][0], &obj->arestas[i][1]);
    }

    fclose(f);

    // --- estado inicial de transformações ---
    obj->transl = (float *)calloc(3, sizeof(float));
    obj->escala = (float *)malloc(3 * sizeof(float));
    obj->escala[0] = obj->escala[1] = obj->escala[2] = 1.0f;
    obj->Rx = obj->Ry = obj->Rz = 0.0f;

    // --- modelMatrix começa como identidade ---
    obj->modelMatrix = alocaMatriz4d();
    criaIdentidade4d(obj->modelMatrix);

    return obj;
}

// -----------------------------------------------------------------------
// Recalcula modelMatrix na ordem: S * Rx * Ry * Rz * T
// (aplica escala, depois rotações, depois translação)
// -----------------------------------------------------------------------
static void recalculaModelMatrix(tObjeto3d *obj) {
    criaIdentidade4d(obj->modelMatrix);

    // --- Escala ---
    float **S = alocaMatriz4d();
    criaIdentidade4d(S);
    S[0][0] = obj->escala[0];
    S[1][1] = obj->escala[1];
    S[2][2] = obj->escala[2];
    multMatriz4d(S, obj->modelMatrix); // modelMatrix = S * I = S
    liberaMatriz4d(S);

    // --- Rotação X ---
    float **Rx = alocaMatriz4d();
    criaIdentidade4d(Rx);
    float cx = cosf(obj->Rx), sx = sinf(obj->Rx);
    Rx[1][1] =  cx; Rx[1][2] = -sx;
    Rx[2][1] =  sx; Rx[2][2] =  cx;
    multMatriz4d(Rx, obj->modelMatrix); // modelMatrix = Rx * modelMatrix
    liberaMatriz4d(Rx);

    // --- Rotação Y ---
    float **Ry = alocaMatriz4d();
    criaIdentidade4d(Ry);
    float cy = cosf(obj->Ry), sy = sinf(obj->Ry);
    Ry[0][0] =  cy; Ry[0][2] =  sy;
    Ry[2][0] = -sy; Ry[2][2] =  cy;
    multMatriz4d(Ry, obj->modelMatrix);
    liberaMatriz4d(Ry);

    // --- Rotação Z ---
    float **Rz = alocaMatriz4d();
    criaIdentidade4d(Rz);
    float cz = cosf(obj->Rz), sz = sinf(obj->Rz);
    Rz[0][0] =  cz; Rz[0][1] = -sz;
    Rz[1][0] =  sz; Rz[1][1] =  cz;
    multMatriz4d(Rz, obj->modelMatrix);
    liberaMatriz4d(Rz);

    // --- Translação ---
    float **T = alocaMatriz4d();
    criaIdentidade4d(T);
    T[0][3] = obj->transl[0];
    T[1][3] = obj->transl[1];
    T[2][3] = obj->transl[2];
    multMatriz4d(T, obj->modelMatrix);
    liberaMatriz4d(T);
}

// -----------------------------------------------------------------------
// API pública de transformações
// -----------------------------------------------------------------------

void escalaObjeto(tObjeto3d *obj, float ex, float ey, float ez) {
    obj->escala[0] = ex;
    obj->escala[1] = ey;
    obj->escala[2] = ez;
    recalculaModelMatrix(obj);
}

void transladaObjeto(tObjeto3d *obj, float tx, float ty, float tz) {
    obj->transl[0] = tx;
    obj->transl[1] = ty;
    obj->transl[2] = tz;
    recalculaModelMatrix(obj);
}

void rotacionaObjetoEixoX(tObjeto3d *obj, float angulo) {
    obj->Rx = angulo;
    recalculaModelMatrix(obj);
}

void rotacionaObjetoEixoY(tObjeto3d *obj, float angulo) {
    obj->Ry = angulo;
    recalculaModelMatrix(obj);
}

void rotacionaObjetoEixoZ(tObjeto3d *obj, float angulo) {
    obj->Rz = angulo;
    recalculaModelMatrix(obj);
}

// -----------------------------------------------------------------------
// Debug
// -----------------------------------------------------------------------
void imprimeObjetoDBG(tObjeto3d *obj) {
    printf("=== Objeto ===\n");
    printf("nPontos: %d  nArestas: %d\n", obj->nPontos, obj->nArestas);
    printf("Translacao: (%.2f, %.2f, %.2f)\n", obj->transl[0], obj->transl[1], obj->transl[2]);
    printf("Escala:     (%.2f, %.2f, %.2f)\n", obj->escala[0], obj->escala[1], obj->escala[2]);
    printf("Rotacao:    Rx=%.2f  Ry=%.2f  Rz=%.2f\n", obj->Rx, obj->Ry, obj->Rz);
    printf("ModelMatrix:\n");
    imprimeMatriz4dDBG(obj->modelMatrix);
}

// -----------------------------------------------------------------------
// Desaloca
// -----------------------------------------------------------------------
void desalocaObjeto(tObjeto3d *obj) {
    for (int i = 0; i < obj->nPontos;  i++) free(obj->pontos[i]);
    for (int i = 0; i < obj->nArestas; i++) free(obj->arestas[i]);
    free(obj->pontos);
    free(obj->arestas);
    free(obj->transl);
    free(obj->escala);
    liberaMatriz4d(obj->modelMatrix);
    free(obj);
}