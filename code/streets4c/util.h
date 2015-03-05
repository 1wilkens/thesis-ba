#ifndef UTIL_H
#define UTIL_H

#include "graph.h"

typedef struct pqueue_t *pqueue;
typedef struct q_elem_t q_elem;

// pqueue_t functions and struct
pqueue pqueue_new(int);
void pqueue_free(pqueue);

void pqueue_push(pqueue, void*, int);
void *pqueue_pop(pqueue);

struct pqueue_t
{
    q_elem *buf;
    int n, cap;
};

struct q_elem_t
{
    void * data;
    int pri;
};

#endif
