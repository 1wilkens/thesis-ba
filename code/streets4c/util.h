#ifndef UTIL_H
#define UTIL_H

typedef struct pqueue_t *pqueue;
typedef struct q_elem_t *q_elem;

// pqueue_t functions and struct
pqueue pqueue_new(int);
void pqueue_free(pqueue);

void pqueue_push(pqueue, q_elem);
q_elem pqueue_pop(pqueue);

struct pqueue_t
{
    q_elem *buf;
    int n, cap;
};

struct q_elem_t
{
    int idx;
    int pri;
};

#endif
