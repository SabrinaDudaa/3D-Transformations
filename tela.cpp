#include "tela.h"
#include <stdlib.h>

SDL_Window *criaTela(char *titulo) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("SDL não inicializou! SDL Erro: %s\n", SDL_GetError());
        return NULL;
    }
    SDL_Window *window = SDL_CreateWindow(
        titulo,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WIDTH, HEIGHT,
        SDL_WINDOW_ALLOW_HIGHDPI
    );
    if (!window)
        printf("SDL não criou a janela! SDL Erro: %s\n", SDL_GetError());
    return window;
}

// Converte coordenada normalizada [-1,1] para pixels e desenha a aresta
void desenhaArestaTela(SDL_Renderer *renderer, float *p1, float *p2) {
    int x1 = (int)(WIDTH  * ((p1[0] + 1.0f) / 2.0f));
    int y1 = (int)(HEIGHT * ((-p1[1] + 1.0f) / 2.0f));
    int x2 = (int)(WIDTH  * ((p2[0] + 1.0f) / 2.0f));
    int y2 = (int)(HEIGHT * ((-p2[1] + 1.0f) / 2.0f));
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void desalocaTela(SDL_Window *window) {
    SDL_DestroyWindow(window);
    SDL_Quit();
}