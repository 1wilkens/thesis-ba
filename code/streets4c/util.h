#ifndef UTIL_H
#define UTIL_H

#include "graph.h"

typedef struct queue_t queue_t;
typedef struct hashtable_t hashtable_t;

// queue_t functions and struct
queue_t *queue_new(int);
void queue_free(queue_t*);

void queue_push(queue_t*, node_t*);
node_t *queue_pop(queue_t*);

struct queue_t
{
    node_t **buf;
    int n, cap;
};

// hashtable_t functions and struct
hashtable_t *hashtable_new(int);
void hashtable_free(hashtable_t*);

void hash_insert(hashtable_t*, long, void*);
void *hash_loopup(hashtable_t*, long);

struct hashtable_t
{
    int cap;
    long *keys;
    void **values;
};

#endif
