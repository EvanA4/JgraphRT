#ifndef ARGS_H
#define ARGS_H

/*
x/t : x pos/dir of camera
y/u : y pos/dir of camera
z/v : z pos/dir of camera
f : camera fov
w : width of picture
h : height of picture
s : task size
m : file name of image
n : number of threads
*/


typedef struct KerrArgs {
    float pos0[3];
    float pos1[3];
    float pos[3];
    float dir[3];
    float fov;
    int width;
    int height;
    int taskSize;
    char *fileName;
    int numThreads;
    char *scene;
} KerrArgs;


int free_args(KerrArgs *args);
KerrArgs *parse_args(int argc, char **argv);


#endif