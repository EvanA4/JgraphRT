#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include "args.h"
#include "tga.h"
#include "render.h"


typedef enum Status {
    BUSY,
    IDLE
} Status;


typedef struct Result {
    Pixel *buf;
    int len;
} Result;


typedef struct Worker {
    pthread_t tid;
    Status stat;
    int startPx;
    Result *result;
} Worker;


typedef struct TPool {
    FILE *fptr;
    Renderer *rptr;
    Worker *workers;
    int size;
    int capacity;
    bool die;
    int taskSize;
    int written;

    pthread_mutex_t mutex;
    pthread_cond_t start;
} TPool;


typedef struct WorkerArgs {
    TPool *pool;
    int widx;
} WorkerArgs;


static Result *gen_pixels(TPool *pool, int widx) {
    int numPxsLeft = pool->rptr->width * pool->rptr->height - pool->workers[widx].startPx;
    int numPxs = numPxsLeft < pool->taskSize ? numPxsLeft : pool->taskSize;

    Result *out = malloc(sizeof(Result));
    out->buf = malloc(numPxs * sizeof(Pixel *));
    out->len = numPxs;

    for (int i = 0; i < numPxs; ++i) {
        int pxNum = pool->workers[widx].startPx + i;
        out->buf[i] = render(pool->rptr, pxNum);
    }

    return out;
}


static void *worker(void *args) {
    WorkerArgs *wargs = (WorkerArgs *) args;
    TPool *pool = wargs->pool;
    Worker *me = pool->workers + wargs->widx;
    // printf("thread created with widx %d!\n", wargs->widx);

    struct timespec maxwait;
    maxwait.tv_nsec = 250000000;
    maxwait.tv_sec = 0;

    while (!pool->die) {
        while (me->stat == IDLE && !pool->die) {
            // wait for signal to start work
            pthread_mutex_lock(&(pool->mutex));
            // printf("locked mutex, waiting for signal!\n");
            pthread_cond_timedwait(&(pool->start), &(pool->mutex), &maxwait);
            pthread_mutex_unlock(&(pool->mutex));
        }
        if (me->startPx < 0) continue;

        // actually do the work
        // printf("aight actually doing work now at pixel %d\n", me->startPx);
        me->result = gen_pixels(pool, wargs->widx);
        me->stat = IDLE;
    }

    // printf("uh oh dying now\n");
    free(wargs);
    return NULL;
}


static void tpool_flush(TPool *pool) {
    // signal all workers to start
    pthread_mutex_lock(&(pool->mutex));
    // printf("locked mutex, broadcasting\n");
    pthread_cond_broadcast(&(pool->start));
    pthread_mutex_unlock(&(pool->mutex));

    // wait for workers to finish
    // printf("waiting for workers to be done\n");
    bool done = false;
    while (!done) {
        done = true;
        usleep(250000);
        for (int i = 0; i < pool->size; ++i) {
            if (pool->workers[i].stat == BUSY) {
                done = false;
                break;
            }
        }
    }

    // write work to tga file
    // printf("workers are done, writing to file...\n");
    for (int i = 0; i < pool->size; ++i) {
        if (pool->workers[i].result) {
            // printf("writing %d pixels at %d\n", pool->workers[i].result->len, pool->workers[i].startPx);
            int startPx = (pool->rptr->width * pool->rptr->height) - (1 + pool->written);
            jgr_write(
                pool->fptr,
                pool->workers[i].result->buf, 
                pool->workers[i].result->len, 
                startPx, 
                pool->rptr->width
            );
            pool->written += pool->workers[i].result->len;
            free(pool->workers[i].result->buf);
            free(pool->workers[i].result);
            pool->workers[i].result = NULL;
            pool->workers[i].startPx = -1;
        }
    }

    // printf("flush was a success!\n");
    pool->capacity = 0;
}


TPool *tpool_init(KerrArgs *args) {
    TPool *pool = malloc(sizeof(TPool));
    pool->die = 0;
    pool->fptr = jgr_open(args->fileName);
    pool->rptr = render_init(args);
    pool->taskSize = args->taskSize;
    pool->size = args->numThreads;
    pool->capacity = 0;
    pool->workers = malloc(args->numThreads * sizeof(Worker));
    pool->written = 0;

    pthread_mutex_init(&(pool->mutex), NULL);
    pthread_cond_init(&(pool->start), NULL);

    // create threads
    for (int i = 0; i < pool->size; ++i) {
        pool->workers[i].result = NULL;
        pool->workers[i].startPx = -1;
        pool->workers[i].stat = IDLE;
        WorkerArgs *wargs = malloc(sizeof(WorkerArgs));
        wargs->pool = pool;
        wargs->widx = i;
        pthread_create(&(pool->workers[i].tid), NULL, worker, (void *) wargs);
    }

    return pool;
}


void tpool_push(TPool *pool, int todo) {
    // add task to array
    // printf("adding task for pixel %d\n", todo);
    pool->workers[pool->capacity].startPx = todo;
    pool->workers[pool->capacity].stat = BUSY;
    pool->capacity += 1;

    // check if filled thread pool
    if (pool->capacity == pool->size) {
        // printf("uh oh filled pool\n");
        tpool_flush(pool);
    }
}


void tpool_close(TPool *pool) {
    // check if there is still work to be done
    if (pool->capacity) {
        tpool_flush(pool);
    }

    // signal all threads to die
    pool->die = true;
    pthread_mutex_lock(&(pool->mutex));
    pthread_cond_broadcast(&(pool->start));
    pthread_mutex_unlock(&(pool->mutex));

    // join each thread
    for (int i = 0; i < pool->size; ++i) {
        pthread_join(pool->workers[i].tid, NULL);
    }

    // free a bunch of stuff
    free(pool->workers);
    pthread_mutex_destroy(&(pool->mutex));
    pthread_cond_destroy(&(pool->start));
    jgr_close(pool->fptr);
    free(pool->rptr);
    free(pool);
}