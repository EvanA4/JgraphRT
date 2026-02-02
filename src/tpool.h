#ifndef TPOOL_H
#define TPOOL_H


#include <stdbool.h>
#include "args.h"


typedef enum Status Status;
typedef struct Task Task;
typedef struct TPool TPool;


TPool *tpool_init(KerrArgs *args);
void tpool_push(TPool *pool, int todo);
void tpool_close(TPool *pool);


#endif