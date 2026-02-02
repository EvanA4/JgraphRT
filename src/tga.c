#include <stdio.h>
#include "tga.h"


FILE *tga_open(int width, int height, const char *fileName) {
    if (width < 1 || height < 1) return NULL;

    static unsigned char tga[18];
    tga[2] = 2;
    tga[12] = 255 & width;
    tga[13] = 255 & (width >> 8);
    tga[14] = 255 & height;
    tga[15] = 255 & (height >> 8);
    tga[16] = 24;
    tga[17] = 32;

    FILE *img = fopen(fileName, "wb");
    if (img) fwrite(tga, sizeof(tga), 1, img);
    return img;
}


void tga_close(FILE *img) {
    if (img) fclose(img);
}


int tga_write(FILE *img, const Pixel *buf, int numPx) {
    return (int) fwrite(buf, numPx, sizeof(Pixel), img);
}