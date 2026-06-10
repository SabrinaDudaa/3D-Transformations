#include "camera.h"
#include "algebra.h"
#include <stdlib.h>
#include <math.h>


static float dot3(float *a, float *b) {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

static void cross3(float *a, float *b, float *out) {
    out[0] = a[1]*b[2] - a[2]*b[1];
    out[1] = a[2]*b[0] - a[0]*b[2];
    out[2] = a[0]*b[1] - a[1]*b[0];
}

static void normalize3(float *v) {
    float len = sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (len > 0.0f) { v[0] /= len; v[1] /= len; v[2] /= len; }
}

static float **alocaMatriz4d() {
    float **m = (float **)malloc(4 * sizeof(float *));
    for (int i = 0; i < 4; i++)
        m[i] = (float *)calloc(4, sizeof(float));
    return m;
}

tCamera3d *criaCamera() {
    tCamera3d *cam = (tCamera3d *)malloc(sizeof(tCamera3d));
    cam->posicao    = (float *)calloc(3, sizeof(float));
    cam->foco       = (float *)calloc(3, sizeof(float));
    cam->cima       = (float *)calloc(3, sizeof(float));
    cam->viewMatrix = alocaMatriz4d();
    criaIdentidade4d(cam->viewMatrix);
    return cam;
}

void defineCamera(tCamera3d *cam,
                  float posX, float posY, float posZ,
                  float focX, float focY, float focZ,
                  float cimX, float cimY, float cimZ) {

    cam->posicao[0] = posX; cam->posicao[1] = posY; cam->posicao[2] = posZ;
    cam->foco[0]    = focX; cam->foco[1]    = focY; cam->foco[2]    = focZ;
    cam->cima[0]    = cimX; cam->cima[1]    = cimY; cam->cima[2]    = cimZ;

    float pos[3] = {posX, posY, posZ};
    float cima[3] = {cimX, cimY, cimZ};

    // forward
    float f[3] = { focX - posX, focY - posY, focZ - posZ };
    normalize3(f);

    // right
    float r[3];
    cross3(f, cima, r);
    normalize3(r);

    // up corrigido
    float u[3];
    cross3(r, f, u);

    float **v = cam->viewMatrix;
    v[0][0] =  r[0]; v[0][1] =  r[1]; v[0][2] =  r[2]; v[0][3] = -dot3(r, pos);
    v[1][0] =  u[0]; v[1][1] =  u[1]; v[1][2] =  u[2]; v[1][3] = -dot3(u, pos);
    v[2][0] = -f[0]; v[2][1] = -f[1]; v[2][2] = -f[2]; v[2][3] =  dot3(f, pos);
    v[3][0] =  0.0f; v[3][1] =  0.0f; v[3][2] =  0.0f; v[3][3] =  1.0f;
}

void desalocaCamera(tCamera3d *cam) {
    free(cam->posicao);
    free(cam->foco);
    free(cam->cima);
    for (int i = 0; i < 4; i++) free(cam->viewMatrix[i]);
    free(cam->viewMatrix);
    free(cam);
}