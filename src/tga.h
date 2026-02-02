#ifndef TGA_H
#define TGA_H


#include <stdio.h>


typedef struct Pixel {
    unsigned char b, g, r;
} Pixel;


FILE *tga_open(int width, int height, const char *fileName);
void tga_close(FILE *img);
int tga_write(FILE *img, const Pixel *buf, int numPx);


#endif