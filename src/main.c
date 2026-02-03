#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "args.h"
#include "tpool.h"
#include "tga.h"


int main(int argc, char **argv) {
    struct KerrArgs *args = parse_args(argc, argv);
    if (!args) return 1;

    const int NUM_STEPS = 60;
    float steps[3] = {
        (args->pos1[0] - args->pos0[0]) / (NUM_STEPS - 1),
        (args->pos1[1] - args->pos0[1]) / (NUM_STEPS - 1),
        (args->pos1[2] - args->pos0[2]) / (NUM_STEPS - 1)
    };

    args->fileName = (char *) malloc(20);
    for (int i = 0; i < NUM_STEPS; ++i) {
        sprintf(args->fileName, "data/%d.jgr", i);
        for (int j = 0; j < 3; ++j) {
            args->pos[j] = args->pos0[j] + steps[j]*i;
        }
        printf("Step %d: {%.2f, %.2f, %.2f}\n", i, args->pos[0], args->pos[1], args->pos[2]);

        TPool *pool = tpool_init(args);
        for (int i = 0; i < args->width * args->height; i += args->taskSize) tpool_push(pool, i);
        tpool_close(pool);
    }

    free_args(args);
}