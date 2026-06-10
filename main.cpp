#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "tela.h"
#include "algebra.h"
#include "objeto.h"
#include "camera.h"
#include "font_embed.h"

typedef enum {
    APP_RUNNING,
    APP_MENU,
    APP_QUIT
} AppState;

typedef enum {
    MENU_CONTROLES = 0,
    MENU_TROCAR,
    MENU_SAIR,
    MENU_COUNT
} MenuItem;

static float *projetaPonto(float *p) {
    float *proj = (float *)malloc(2 * sizeof(float));
    float d = (p[2] != 0.0f) ? -p[2] : 0.001f;
    proj[0] = p[0] / d;
    proj[1] = p[1] / d;
    return proj;
}

void desenhaObjetoTela(SDL_Renderer *renderer, float **viewMatrix, tObjeto3d *objeto) {
    for (int i = 0; i < objeto->nArestas; i++) {
        int idx0 = objeto->arestas[i][0];
        int idx1 = objeto->arestas[i][1];

        float *m0 = multMatriz4dPonto(objeto->modelMatrix, objeto->pontos[idx0]);
        float *m1 = multMatriz4dPonto(objeto->modelMatrix, objeto->pontos[idx1]);

        float *v0 = multMatriz4dPonto(viewMatrix, m0);
        float *v1 = multMatriz4dPonto(viewMatrix, m1);

        float *p0 = projetaPonto(v0);
        float *p1 = projetaPonto(v1);

        desenhaArestaTela(renderer, p0, p1);

        free(m0); free(m1);
        free(v0); free(v1);
        free(p0); free(p1);
    }
}


static void drawRect(SDL_Renderer *r, int x, int y, int w, int h, SDL_Color c, int filled) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    SDL_Rect rect = {x, y, w, h};
    if (filled) SDL_RenderFillRect(r, &rect);
    else        SDL_RenderDrawRect(r, &rect);
}

static void drawText(SDL_Renderer *renderer, TTF_Font *font, const char *text,
                     int x, int y, SDL_Color color, int centerX) {
    if (!font || !text) return;
    SDL_Surface *surf = TTF_RenderUTF8_Blended(font, text, color);
    if (!surf) return;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    if (!tex) { SDL_FreeSurface(surf); return; }

    int tw = surf->w, th = surf->h;
    SDL_FreeSurface(surf);

    SDL_Rect dst = {centerX ? x - tw/2 : x, y - th/2, tw, th};
    SDL_RenderCopy(renderer, tex, NULL, &dst);
    SDL_DestroyTexture(tex);
}

static void desenhaMenu(SDL_Renderer *renderer, TTF_Font *fontGrande, TTF_Font *fontMedia,
                        TTF_Font *fontPequena, MenuItem selecionado, int mostraControles,
                        const char *arquivoAtual) {
    // Overlay escuro semitransparente
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 10, 10, 20, 200);
    SDL_Rect full = {0, 0, WIDTH, HEIGHT};
    SDL_RenderFillRect(renderer, &full);

    // Painel central
    int pw = 420, ph = 320;
    int px = (WIDTH  - pw) / 2;
    int py = (HEIGHT - ph) / 2;

    // Sombra do painel
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 120);
    SDL_Rect shadow = {px+6, py+6, pw, ph};
    SDL_RenderFillRect(renderer, &shadow);

    // Fundo do painel
    SDL_Color corPainel = {22, 27, 44, 255};
    drawRect(renderer, px, py, pw, ph, corPainel, 1);

    // Borda do painel
    SDL_Color corBorda = {70, 130, 200, 255};
    drawRect(renderer, px, py, pw, ph, corBorda, 0);
    drawRect(renderer, px+1, py+1, pw-2, ph-2, corBorda, 0);

    // Linha decorativa superior
    SDL_SetRenderDrawColor(renderer, 70, 130, 200, 255);
    SDL_RenderDrawLine(renderer, px+20, py+52, px+pw-20, py+52);

    SDL_Color branco = {255, 255, 255, 255};
    SDL_Color cinza  = {160, 170, 190, 255};
    SDL_Color azul   = { 70, 130, 200, 255};

    if (!mostraControles) {
        // === Tela principal do menu ===

        drawText(renderer, fontGrande, "MENU", px + pw/2, py + 30, branco, 1);

        char infoArq[128];
        snprintf(infoArq, sizeof(infoArq), "Arquivo: %s", arquivoAtual ? arquivoAtual : "?");
        drawText(renderer, fontPequena, infoArq, px + pw/2, py + 65, cinza, 1);

        const char *labels[MENU_COUNT] = {
            "  Controles",
            "  Trocar Arquivo",
            "  Sair"
        };
        const char *hints[MENU_COUNT] = {
            "Ver teclas de atalho",
            "Carregar outro .dcg",
            "Encerrar o programa"
        };

        int itemH  = 58;
        int itemW  = pw - 60;
        int itemX  = px + 30;
        int firstY = py + 95;

        for (int i = 0; i < MENU_COUNT; i++) {
            int iy     = firstY + i * itemH;
            int centro = iy + itemH/2;

            if (i == (int)selecionado) {
                SDL_Color sel = {40, 80, 140, 255};
                drawRect(renderer, itemX, iy+4, itemW, itemH-8, sel, 1);
                SDL_Color selBorda = {100, 160, 240, 255};
                drawRect(renderer, itemX, iy+4, itemW, itemH-8, selBorda, 0);

                SDL_SetRenderDrawColor(renderer, 100, 200, 255, 255);
                for (int k = 0; k < 4; k++)
                    SDL_RenderDrawLine(renderer, itemX, iy+4+k, itemX, iy+itemH-4+k);

                drawText(renderer, fontMedia,   labels[i], itemX + 20, centro - 8,  branco, 0);
                drawText(renderer, fontPequena, hints[i],  itemX + 20, centro + 14, azul,   0);
            } else {
                drawText(renderer, fontMedia,   labels[i], itemX + 20, centro - 8,  cinza, 0);
                drawText(renderer, fontPequena, hints[i],  itemX + 20, centro + 14,
                         (SDL_Color){80, 100, 130, 255}, 0);
            }
        }

        drawText(renderer, fontPequena, "↑↓ Navegar   Enter Confirmar   ESC Voltar",
                 px + pw/2, py + ph - 18, (SDL_Color){90, 100, 120, 255}, 1);

    } else {
        // === Tela de controles ===
        drawText(renderer, fontGrande, "CONTROLES", px + pw/2, py + 30, branco, 1);

        SDL_SetRenderDrawColor(renderer, 70, 130, 200, 255);
        SDL_RenderDrawLine(renderer, px+20, py+52, px+pw-20, py+52);

        struct { const char *tecla; const char *acao; } ctrl[] = {
            {"Q / E",  "Rotacao eixo X"},
            {"A / D",  "Rotacao eixo Y"},
            {"Z / C",  "Rotacao eixo Z"},
            {"Setas",  "Translacao X / Y"},
            {"R / F",  "Translacao Z"},
            {"+ / -",  "Escala uniforme"},
            {"ESC",    "Abrir este menu"},
        };
        int n = sizeof(ctrl) / sizeof(ctrl[0]);

        int startY = py + 75;
        int lineH  = 28;

        SDL_Color amarelo = {255, 210, 80, 255};
        for (int i = 0; i < n; i++) {
            int ly = startY + i * lineH;
            drawText(renderer, fontMedia,   ctrl[i].tecla, px + 30,       ly, amarelo, 0);
            drawText(renderer, fontPequena, ctrl[i].acao,  px + 30 + 130, ly, cinza,   0);
        }

        drawText(renderer, fontPequena, "ESC ou Enter para voltar",
                 px + pw/2, py + ph - 18, (SDL_Color){90, 100, 120, 255}, 1);
    }
}

static int dialogTrocarArquivo(SDL_Renderer *renderer, TTF_Font *fontGrande,
                                TTF_Font *fontMedia, TTF_Font *fontPequena,
                                char *bufOut, int bufSize) {
    char entrada[256] = "";
    int  len = 0;
    SDL_StartTextInput();

    SDL_Color branco   = {255, 255, 255, 255};
    SDL_Color cinza    = {160, 170, 190, 255};
    SDL_Color amarelo  = {255, 210,  80, 255};
    SDL_Color vermelho = {220,  80,  80, 255};

    const char *erro = NULL;

    SDL_Event ev;
    while (1) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) { SDL_StopTextInput(); return -1; }

            if (ev.type == SDL_KEYDOWN) {
                SDL_Keycode k = ev.key.keysym.sym;
                if (k == SDLK_ESCAPE) { SDL_StopTextInput(); return 0; }
                if (k == SDLK_RETURN || k == SDLK_KP_ENTER) {
                    if (len == 0) { erro = "Digite um nome de arquivo!"; continue; }
                    strncpy(bufOut, entrada, bufSize - 1);
                    bufOut[bufSize - 1] = '\0';
                    SDL_StopTextInput();
                    return 1;
                }
                if (k == SDLK_BACKSPACE && len > 0) {
                    entrada[--len] = '\0';
                    erro = NULL;
                }
            }
            if (ev.type == SDL_TEXTINPUT) {
                int add = (int)strlen(ev.text.text);
                if (len + add < (int)sizeof(entrada) - 1) {
                    strcat(entrada, ev.text.text);
                    len += add;
                    erro = NULL;
                }
            }
        }

        // Fundo
        SDL_SetRenderDrawColor(renderer, 10, 10, 20, 255);
        SDL_Rect full = {0, 0, WIDTH, HEIGHT};
        SDL_RenderFillRect(renderer, &full);

        int pw = 440, ph = 240;
        int px = (WIDTH  - pw) / 2;
        int py = (HEIGHT - ph) / 2;

        // Sombra
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
        SDL_Rect sh = {px+6, py+6, pw, ph};
        SDL_RenderFillRect(renderer, &sh);

        // Painel
        SDL_SetRenderDrawColor(renderer, 22, 27, 44, 255);
        SDL_Rect panel = {px, py, pw, ph};
        SDL_RenderFillRect(renderer, &panel);
        SDL_SetRenderDrawColor(renderer, 70, 130, 200, 255);
        SDL_RenderDrawRect(renderer, &panel);

        drawText(renderer, fontGrande, "TROCAR ARQUIVO", px + pw/2, py + 30, branco, 1);

        SDL_SetRenderDrawColor(renderer, 70, 130, 200, 255);
        SDL_RenderDrawLine(renderer, px+20, py+52, px+pw-20, py+52);

        drawText(renderer, fontPequena, "Caminho do arquivo .dcg:", px + 30, py + 78, cinza, 0);

        // Caixa de texto
        SDL_SetRenderDrawColor(renderer, 35, 40, 60, 255);
        SDL_Rect caixa = {px+30, py+92, pw-60, 36};
        SDL_RenderFillRect(renderer, &caixa);
        SDL_SetRenderDrawColor(renderer, 100, 150, 220, 255);
        SDL_RenderDrawRect(renderer, &caixa);

        // Texto digitado + cursor piscante
        char display[260];
        Uint32 t = SDL_GetTicks();
        if ((t / 500) % 2 == 0)
            snprintf(display, sizeof(display), "%s|", entrada);
        else
            snprintf(display, sizeof(display), "%s", entrada);

        drawText(renderer, fontMedia, display, px + 40, py + 92 + 18, amarelo, 0);

        if (erro)
            drawText(renderer, fontPequena, erro, px + pw/2, py + 145, vermelho, 1);
        else
            drawText(renderer, fontPequena, "Ex: cubo.dcg  ou  /caminho/objeto.dcg",
                     px + pw/2, py + 145, (SDL_Color){80, 100, 130, 255}, 1);

        drawText(renderer, fontPequena, "Enter Confirmar   ESC Cancelar",
                 px + pw/2, py + ph - 18, (SDL_Color){90, 100, 120, 255}, 1);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo.dcg>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char arquivoAtual[512];
    strncpy(arquivoAtual, argv[1], sizeof(arquivoAtual) - 1);
    arquivoAtual[sizeof(arquivoAtual) - 1] = '\0';

    // --- Carrega objeto ---
    tObjeto3d *objeto = carregaObjeto(arquivoAtual);
    if (!objeto) return EXIT_FAILURE;
    imprimeObjetoDBG(objeto);

    // --- Câmera ---
    tCamera3d *camera = criaCamera();
    defineCamera(camera, 0.0f, 0.0f, 50.0f,
                         0.0f, 0.0f,  0.0f,
                         0.0f, 1.0f,  0.0f);

    // --- SDL + TTF ---
    SDL_Window   *window   = criaTela((char *)"T2 - Transformacoes 3D");
    if (!window) return EXIT_FAILURE;
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (TTF_Init() < 0) {
        printf("SDL_ttf nao inicializou: %s\n", TTF_GetError());
        return EXIT_FAILURE;
    }

    // Carrega fonte embutida (gerada por gera_font.py — independente do sistema)
    TTF_Font *fontGrande  = NULL;
    TTF_Font *fontMedia   = NULL;
    TTF_Font *fontPequena = NULL;

    {
        SDL_RWops *rw;

        rw = SDL_RWFromConstMem(font_embed_data, (int)font_embed_size);
        if (rw) { fontGrande  = TTF_OpenFontRW(rw, 1, 22); }

        rw = SDL_RWFromConstMem(font_embed_data, (int)font_embed_size);
        if (rw) { fontMedia   = TTF_OpenFontRW(rw, 1, 16); }

        rw = SDL_RWFromConstMem(font_embed_data, (int)font_embed_size);
        if (rw) { fontPequena = TTF_OpenFontRW(rw, 1, 13); }
    }
    if (!fontGrande)
        printf("Aviso: falha ao carregar fonte embutida: %s\n", TTF_GetError());

    // --- Estado ---
    float angX = 0, angY = 0, angZ = 0;
    float tx   = 0, ty   = 0, tz   = 0;
    float escala = 1.0f;
    const float PR = 0.05f, PT = 0.5f, PE = 0.1f;

    AppState appState        = APP_RUNNING;
    MenuItem menuSelecionado = MENU_CONTROLES;
    int      mostraControles = 0;

    printf("\n=== Controles ===\n");
    printf("Rotacao:    Q/E (X)  A/D (Y)  Z/C (Z)\n");
    printf("Translacao: Setas (XY)  R/F (Z)\n");
    printf("Escala:     +/-\n");
    printf("Menu:       ESC\n\n");

    SDL_Event ev;

    while (appState != APP_QUIT) {

        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) { appState = APP_QUIT; break; }

            if (ev.type == SDL_KEYDOWN) {
                SDL_Keycode k = ev.key.keysym.sym;

                if (appState == APP_RUNNING) {
                    switch (k) {
                        case SDLK_q:      angX -= PR; break;
                        case SDLK_e:      angX += PR; break;
                        case SDLK_a:      angY -= PR; break;
                        case SDLK_d:      angY += PR; break;
                        case SDLK_z:      angZ -= PR; break;
                        case SDLK_c:      angZ += PR; break;
                        case SDLK_LEFT:   tx   -= PT; break;
                        case SDLK_RIGHT:  tx   += PT; break;
                        case SDLK_UP:     ty   += PT; break;
                        case SDLK_DOWN:   ty   -= PT; break;
                        case SDLK_r:      tz   += PT; break;
                        case SDLK_f:      tz   -= PT; break;
                        case SDLK_PLUS:
                        case SDLK_EQUALS: escala += PE; break;
                        case SDLK_MINUS:
                            escala -= PE;
                            if (escala < 0.01f) escala = 0.01f;
                            break;
                        case SDLK_ESCAPE:
                            appState        = APP_MENU;
                            menuSelecionado = MENU_CONTROLES;
                            mostraControles = 0;
                            break;
                        default: break;
                    }

                    if (appState == APP_RUNNING) {
                        rotacionaObjetoEixoX(objeto, angX);
                        rotacionaObjetoEixoY(objeto, angY);
                        rotacionaObjetoEixoZ(objeto, angZ);
                        transladaObjeto(objeto, tx, ty, tz);
                        escalaObjeto(objeto, escala, escala, escala);
                    }

                } else if (appState == APP_MENU) {
                    if (mostraControles) {
                        if (k == SDLK_ESCAPE || k == SDLK_RETURN || k == SDLK_KP_ENTER)
                            mostraControles = 0;
                    } else {
                        switch (k) {
                            case SDLK_UP:
                                menuSelecionado = (MenuItem)((menuSelecionado - 1 + MENU_COUNT) % MENU_COUNT);
                                break;
                            case SDLK_DOWN:
                                menuSelecionado = (MenuItem)((menuSelecionado + 1) % MENU_COUNT);
                                break;
                            case SDLK_ESCAPE:
                                appState = APP_RUNNING;
                                break;
                            case SDLK_RETURN:
                            case SDLK_KP_ENTER:
                                if (menuSelecionado == MENU_CONTROLES) {
                                    mostraControles = 1;

                                } else if (menuSelecionado == MENU_TROCAR) {
                                    char novoArq[512] = "";
                                    int res = dialogTrocarArquivo(renderer,
                                                  fontGrande, fontMedia, fontPequena,
                                                  novoArq, sizeof(novoArq));
                                    if (res == 1) {
                                        tObjeto3d *novoObj = carregaObjeto(novoArq);
                                        if (novoObj) {
                                            desalocaObjeto(objeto);
                                            objeto = novoObj;
                                            strncpy(arquivoAtual, novoArq, sizeof(arquivoAtual) - 1);
                                            arquivoAtual[sizeof(arquivoAtual) - 1] = '\0';
                                            angX = angY = angZ = tx = ty = tz = 0.0f;
                                            escala   = 1.0f;
                                            appState = APP_RUNNING;
                                        } else {
                                            printf("Erro ao carregar: %s\n", novoArq);
                                        }
                                    }
                                    // res == 0: cancelado, permanece no menu

                                } else if (menuSelecionado == MENU_SAIR) {
                                    appState = APP_QUIT;
                                }
                                break;
                            default: break;
                        }
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 220, 100, 255);
        desenhaObjetoTela(renderer, camera->viewMatrix, objeto);

        if (appState == APP_MENU) {
            desenhaMenu(renderer, fontGrande, fontMedia, fontPequena,
                        menuSelecionado, mostraControles, arquivoAtual);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // --- Limpeza ---
    if (fontGrande)  TTF_CloseFont(fontGrande);
    if (fontMedia)   TTF_CloseFont(fontMedia);
    if (fontPequena) TTF_CloseFont(fontPequena);
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    desalocaTela(window);
    desalocaObjeto(objeto);
    desalocaCamera(camera);

    return EXIT_SUCCESS;
}