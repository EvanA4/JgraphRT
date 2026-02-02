#ifndef RENDER_H
#define RENDER_H

#include "tga.h"
#include "args.h"


typedef float Mat4[4][4];
typedef float Vec2[2];
typedef float Vec3[3];
typedef float Vec4[4];
typedef struct Renderer {
    float pos[3];
    float dir[3];
    float fov;
    int width;
    int height;
    Mat4 view;
    char *scene;
} Renderer;


Renderer *render_init(KerrArgs *args);
Pixel render(Renderer *rptr, int px);


#endif