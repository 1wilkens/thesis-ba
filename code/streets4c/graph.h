#ifndef GRAPH_H
#define GRAPH_H

#include <glib.h>
#include "util.h"

#define INFINITY INT_MAX // redefine infinity to something useful

#define N_ID_TO_IDX(g, id) GPOINTER_TO_INT(g_hash_table_lookup(g->node_idx, (void*)id)) // useful coversion macros
#define E_ID_TO_IDX(g, id) GPOINTER_TO_INT(g_hash_table_lookup(g->edge_idx, (void*)id))

// typedefs
typedef struct node_t *node;
typedef struct edge_t *edge;
typedef struct graph_t *graph;

typedef struct dgraph_t *dgraph;

// node related functions
node new_node();
void free_node(node);

// edge related functions
edge new_edge();
void free_edge(edge);

// graph related functions
void print_graph(graph);
graph new_graph(int, int);
void free_graph(graph);

void add_node(graph, node);
void add_edge(graph, long, long, edge);

// dijkstra functions
dgraph new_dgraph(graph);
void free_dgraph();

void dijkstra(dgraph, int);

// other util functions/macros
#define RAD(r) (r * 3.14159265358979323846 / 180.0) // M_PI is dropped in C99...

double haversine_length(node, node);

// structs
struct node_t
{
    long osm_id;

    double lon, lat;

    // adj[node] = edge
    GHashTable *adj;
};

struct edge_t
{
    long osm_id;

    int length;
    int max_speed;

    int driving_time; // == weight
};

struct graph_t
{
    int n_nodes, n_edges;

    node *nodes;
    edge *edges;

    // node_idx[long] = int
    GHashTable *node_idx;
    // edge_idx[long] = int
    GHashTable *edge_idx;
};

struct dgraph_t
{
    // base graph to work on
    graph g;
    // priority queue to store nodes to explore
    pqueue pq;

    // current node to explore
    int cur;

    // dist[node] = int
    int *dist;

    // parents[node] = node
    int *parents;
};

#endif
