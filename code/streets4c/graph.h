#ifndef GRAPH_H
#define GRAPH_H

typedef struct queue_t queue_t;

typedef struct node_t node_t;
typedef struct edge_t edge_t;

queue_t *new_queue(int);
void queue_free(queue_t*);

void queue_push(queue_t*, node_t*);
node_t *queue_pop(queue_t*);

struct queue_t
{
    node_t **buf;
    int n, cap;
};


struct node_t
{
    // general
    node_t *pred;
    int cost;

    // osm
    long id;
    long lon, lat;
};

struct edge_t
{
    // general
    node_t *from, *to;
    int weight;

    // osm
    long id;
};

#endif
