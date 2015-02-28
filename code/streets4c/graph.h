#ifndef GRAPH_H
#define GRAPH_H

// typedefs
typedef struct queue_t queue_t;

typedef struct node_t node_t;
typedef struct edge_t edge_t;
typedef struct graph_t graph_t;

// queue_t functions and struct
queue_t *new_queue(int);
void queue_free(queue_t*);

void queue_push(queue_t*, node_t*);
node_t *queue_pop(queue_t*);

struct queue_t
{
    node_t **buf;
    int n, cap;
};

// graph related functions and structs
graph_t *new_graph(int);
void free_graph(graph_t*);

void add_node(graph_t*, node_t*);
void add_edge(graph_t*, edge_t*);
node_t *dijkstra(graph_t*, node_t*, long);

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

struct graph_t
{
    node_t **nodes;
    edge_t **edges;
};

#endif
