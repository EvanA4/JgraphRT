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
    
    TPool *pool = tpool_init(args);
    for (int i = 0; i < args->width * args->height; i += args->taskSize) tpool_push(pool, i);
    tpool_close(pool);

    free_args(args);
}