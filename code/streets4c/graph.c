#include "graph.h"
#include <stdlib.h>

// taken and modified from http://rosettacode.org/wiki/Priority_queue#C

queue_t *new_queue(int cap)
{
    queue_t *q = malloc(sizeof(queue_t));
    q->buf = malloc(sizeof(node_t*) * cap);
    q->cap = cap;
    q->n = 1;

    return q;
}

void queue_free(queue_t *q)
{
    free(q->buf);
    free(q);
}

void queue_push(queue_t *q, node_t* node)
{
    if (q->n >= q->cap)
    {
        q->cap *= 2;
        q->buf = realloc(q->buf, sizeof(node_t) * q->cap);
    }

    int n = q->n++;
    int m = 0;
    while ((m = n / 2) && node->cost < q->buf[m]->cost)
    {
        q->buf[n] = q->buf[m];
        n = m;
    }
    q->buf[n] = node;
}

node_t *queue_pop(queue_t *q)
{
    if (q->n == 1)
    {
        return NULL;
    }

    node_t *res = q->buf[1];

    /* pull last item to top, then down heap. */
    q->n -= 1;

    int n = 1, m = 0;
    while ((m = n * 2) < q->n)
    {
        if (m + 1 < q->n && q->buf[m]->cost > q->buf[m + 1]->cost)
        {
            m++;
        }

        if (q->buf[q->n]->cost <= q->buf[m]->cost)
        {
            break;
        }
        q->buf[n] = q->buf[m];
        n = m;
    }

    q->buf[n] = q->buf[q->n];

    return res;
}
