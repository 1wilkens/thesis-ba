#include<stdlib.h>

#include "util.h"

// taken and modified from http://rosettacode.org/wiki/Priority_queue#C
queue_t *queue_new(int cap)
{
    queue_t *q = calloc(1, sizeof(queue_t));
    q->buf = calloc(cap, sizeof(node_t*));
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

hashtable_t *hashtable_new(int capacity) {
    hashtable_t *h = calloc(1, sizeof (hashtable_t));
    h->cap = capacity;
    h->keys = calloc(capacity, sizeof(long));
    h->values = calloc(capacity, sizeof(void *));
    return h;
}

void hashtable_free(hashtable_t *h)
{
    free(h->values);
    free(h->keys);
    free(h);
}

static int hash_index(hashtable_t *h, long key)
{
    int index = key % h->cap;
    while (h->keys[index] && h->keys[index] != key)
    {
        index = (index + 1) % h->cap;
    }
    return index;
}

void hash_insert(hashtable_t *h, long key, void *value)
{
    int index = hash_index(h, key);
    h->keys[index] = key;
    h->values[index] = value;
}

void *hash_lookup(hashtable_t *h, long key)
{
    int index = hash_index(h, key);
    return h->values[index];
}
