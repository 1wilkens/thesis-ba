#include<stdlib.h>

#include "util.h"

// taken and modified from http://rosettacode.org/wiki/Priority_queue#C
pqueue pqueue_new(int cap)
{
    pqueue q = malloc(sizeof(struct pqueue_t));
    q->buf = malloc(cap * sizeof(q_elem));
    q->cap = cap;
    q->n = 1;

    return q;
}

void pqueue_free(pqueue q)
{
    free(q->buf);
    free(q);
}

void pqueue_push(pqueue q, void *data, int pri)
{
    q_elem *b = q->buf;

    int n = q->n++;
    int m = 0;
    /* append at end, then up heap */
    while ((m = n / 2) && pri < b[m].pri)
    {
        b[n] = b[m];
        n = m;
    }
    b[n].data = data;
    b[n].pri = pri;
}

void *pqueue_pop(pqueue q)
{
    if (q->n == 1) return 0;

    q_elem *b = q->buf;

    void *result = b[1].data;

    /* pull last item to top, then down heap. */
    --q->n;

    int n = 1, m;
    while ((m = n * 2) < q->n)
    {
        if (m + 1 < q->n && b[m].pri > b[m + 1].pri) m++;

        if (b[q->n].pri <= b[m].pri) break;

        b[n] = b[m];
        n = m;
    }

    b[n] = b[q->n];
    return result;
}
