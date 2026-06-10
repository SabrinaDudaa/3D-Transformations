#ifndef tObj
#define tObj

#include <stdio.h>
#include <SDL2/SDL.h>

typedef struct _tObj {
    int nPontos;
    int nArestas;
    float **pontos;
    int **arestas;
    float *transl;
    float *escala;
    float Rx;
    float Ry;
    float Rz;
    float **modelMatrix;
} tObjeto3d;

// Lê as informações de um arquivo e as carrega num novo objeto alocado
tObjeto3d *carregaObjeto(char *nomeArquivo);

// Altera a modelMatrix de um objeto para redimensioná-lo segundo escalaX, escalaY e escalaZ
void escalaObjeto(tObjeto3d *objeto, float escalaX, float escalaY, float escalaZ);

// Altera a modelMatrix de um objeto para transladá-lo segundo transX, transY e transZ
void transladaObjeto(tObjeto3d *objeto, float transX, float transY, float transZ);

// Altera a modelMatrix de um objeto para rotacioná-lo ao redor do eixo X segundo o angulo informado
void rotacionaObjetoEixoX(tObjeto3d *objeto, float angulo);

// Altera a modelMatrix de um objeto para rotacioná-lo ao redor do eixo Y segundo o angulo informado
void rotacionaObjetoEixoY(tObjeto3d *objeto, float angulo);

// Altera a modelMatrix de um objeto para rotacioná-lo ao redor do eixo Z segundo o angulo informado
void rotacionaObjetoEixoZ(tObjeto3d *objeto, float angulo);

// Imprime um objeto no terminal
void imprimeObjetoDBG(tObjeto3d *objeto);

// Desaloca o objeto
void desalocaObjeto(tObjeto3d *objeto);

#endif // tObj