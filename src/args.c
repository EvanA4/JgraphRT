#include <getopt.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "args.h"


int free_args(KerrArgs *args) {
    if (!args) return 0;
    if (args->fileName) free(args->fileName);
    free(args);
    return 1;
}


static void print_usage() {
    fprintf(
        stderr,
        "Usage: bin/kerr [schwarz|sphere]\n"
        "Flags:\n"
        "\ta/x: %35s\n"
        "\tb/y: %35s\n"
        "\tc/z: %35s\n"
        "\tt:   %35s\n"
        "\tu:   %35s\n"
        "\tv:   %35s\n"
        "\tf:   %35s\n"
        "\tq:   %35s\n"
        "\ts:   %35s\n"
        "\tn:   %35s\n",
        "x for start/ending pos of camera",
        "y for start/ending pos of camera",
        "z for start/ending pos of camera",
        "x for dir of camera",
        "y for dir of camera",
        "z for dir of camera",
        "camera fov",
        "number of steps in GIF",
        "task size",
        "number of threads"
    ); 
}


static void print_args(KerrArgs *args) {
    printf(
        "Start Position: {%.2f, %.2f, %.2f}\n"
        "End Position: {%.2f, %.2f, %.2f}\n"
        "Direction: {%.2f, %.2f, %.2f}\n"
        "FOV: %.2f\n"
        "Image Size: %d x %d\n"
        "Pixels per Task: %d\n"
        "Number of Threads: %d\n"
        "Scene: %s\n",
        args->pos0[0], args->pos0[1], args->pos0[2],
        args->pos1[0], args->pos1[1], args->pos1[2],
        args->dir[0], args->dir[1], args->dir[2],
        args->fov,
        args->width, args->height,
        args->taskSize,
        args->numThreads,
        args->scene
    );
}


KerrArgs *parse_args(int argc, char **argv) {
    KerrArgs *out = malloc(sizeof(KerrArgs));
    *out = (KerrArgs) {
        {1.1, .1, -8},
        {1.1, .1, 8},
        {0, 0, 0},
        {0, 0, 1},
        90,         // fov
        30,         // num steps
        96,         // width
        54,         // height
        2048,       // task size
        NULL,       // file name
        16,         // num threads
        "schwarz"   // scene
    };

    // get the scene
    if (argc < 2) {
        print_usage();
        free(out);
        return NULL;
    } else {
        out->scene = argv[1];
        if (strcmp(out->scene, "schwarz") && strcmp(out->scene, "sphere")) {
            fprintf(stderr, "Error: invalid scene \"%s\" name is neither \"schwarz\" nor \"sphere\"\n", out->scene);
            free(out);
            return NULL;
        }
    }

    int opt;
    while((opt = getopt(argc - 1, argv + 1, "a:b:c:x:t:y:u:z:v:f:q:s:n:")) != -1)  
    {  
        switch(opt)  
        {
            case 'a':
                if (sscanf(optarg, "%f", &(out->pos0[0])) != 1) {
                    fprintf(stderr, "Error: failed to convert x position to a float\n");
                    free_args(out);
                    return NULL;
                }
                break;

            case 'b':
                if (sscanf(optarg, "%f", &(out->pos0[1])) != 1) {
                    fprintf(stderr, "Error: failed to convert y position to a float\n");
                    free_args(out);
                    return NULL;
                }
                break;

            case 'c':
                if (sscanf(optarg, "%f", &(out->pos0[2])) != 1) {
                    fprintf(stderr, "Error: failed to convert z position to a float\n");
                    free_args(out);
                    return NULL;
                }
                break;

            case 'x':
                if (sscanf(optarg, "%f", &(out->pos1[0])) != 1) {
                    fprintf(stderr, "Error: failed to convert x position to a float\n");
                    free_args(out);
                    return NULL;
                }
                break;

            case 'y':
                if (sscanf(optarg, "%f", &(out->pos1[1])) != 1) {
                    fprintf(stderr, "Error: failed to convert y position to a float\n");
                    free_args(out);
                    return NULL;
                }
                break;

            case 'z':
                if (sscanf(optarg, "%f", &(out->pos1[2])) != 1) {
                    fprintf(stderr, "Error: failed to convert z position to a float\n");
                    free_args(out);
                    return NULL;
                }
                break;
            
            case 't':
                if (sscanf(optarg, "%f", &(out->dir[0])) != 1) {
                    fprintf(stderr, "Error: failed to convert x direction to a float\n");
                    free_args(out);
                    return NULL;
                }
                break;
            
            case 'u':
                if (sscanf(optarg, "%f", &(out->dir[1])) != 1) {
                    fprintf(stderr, "Error: failed to convert y direction to a float\n");
                    free_args(out);
                    return NULL;
                }
                break;

            case 'v':
                if (sscanf(optarg, "%f", &(out->dir[2])) != 1) {
                    fprintf(stderr, "Error: failed to convert z direction to a float\n");
                    free_args(out);
                    return NULL;
                }
                break;

            case 'f':
                if (sscanf(optarg, "%f", &(out->fov)) != 1) {
                    fprintf(stderr, "Error: failed to convert FOV to a float\n");
                    free_args(out);
                    return NULL;
                }
                break;

            case 'q':
                if (sscanf(optarg, "%d", &(out->num_steps)) != 1) {
                    fprintf(stderr, "Error: failed to convert number of steps to an integer\n");
                    free_args(out);
                    return NULL;
                }
                if (out->num_steps <= 0) {
                    fprintf(stderr, "Error: invalid number of steps\n");
                    free_args(out);
                    return NULL;
                }
                break;

            case 's':
                if (sscanf(optarg, "%d", &(out->taskSize)) != 1) {
                    fprintf(stderr, "Error: failed to convert pixels per task to an integer\n");
                    free_args(out);
                    return NULL;
                }
                break;

            case 'n':  
                if (sscanf(optarg, "%d", &(out->numThreads)) != 1) {
                    fprintf(stderr, "Error: failed to convert number of threads to an integer\n");
                    free_args(out);
                    return NULL;
                }
                break;

            case '?':  
                print_usage();
                free_args(out);
                return NULL;
        }  
    }

    // normalize direction vector
    float dirlen = sqrt(out->dir[0] * out->dir[0] + out->dir[1] * out->dir[1] + out->dir[2] * out->dir[2]);
    if (dirlen > .001) for (int i = 0; i < 3; ++i) out->dir[i] /= dirlen;

    print_args(out);
    return out;
}