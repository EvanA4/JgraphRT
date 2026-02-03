#include <stdio.h>
#include <string.h>
#include "tga.h"


FILE *jgr_open(const char *fileName) {
    FILE *jgr = fopen(fileName, "w");
    char header[] = "newgraph\nxaxis size 4.8 nodraw\nyaxis size 2.7 nodraw\n\n";
    if (jgr) fwrite(header, (int) strlen(header), 1, jgr);
    return jgr;
}


void jgr_close(FILE *img) {
    if (img) fclose(img);
}


int jgr_write(FILE *img, const Pixel *buf, int numPx, int startPx, int width, int height) {
    // newline poly pcfill 255 255 255 pts 0 0  1 0  1 1  0 1 color 255 255 255
    int written = 0;
    for (int i = 0; i < numPx; ++i) {
        char line[100];
        int pxID = startPx-i;
        int x = width-1-pxID%width;
        int y = pxID/width;
        int line_len = sprintf(line, "newline poly pts %d %d  %d %d  %d %d  %d %d color %.2f %.2f %.2f\n",
            x,
            y,
            x+1,
            y,
            x+1,
            y+1,
            x,
            y+1,
            ((double) buf[i].r) / 255.,
            ((double) buf[i].g) / 255.,
            ((double) buf[i].b) / 255.
        );
        written += fwrite(line, 1, line_len, img);
    }
    return written;
}