#ifndef TGA_H
#define TGA_H


#include <stdio.h>


typedef struct Pixel {
    unsigned char b, g, r;
} Pixel;


FILE *jgr_open(const char *fileName);
void jgr_close(FILE *img);
int jgr_write(FILE *img, const Pixel *buf, int numPx, int startPx, int width);


#endif