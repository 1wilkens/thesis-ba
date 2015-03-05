#ifndef GRAPH_H
#define GRAPH_H

#include <glib.h>

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
graph new_graph(int);
void free_graph(graph);

void add_node(graph, node);
void add_edge(graph, node, node, edge);

// dijkstra functions
dgraph dgraph_new(graph);
void free_dgraph();

void dijkstra(dgraph, node);

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
    int max_speed, actual_speed;
};

struct graph_t
{
    int n_nodes, n_edges;
    node *nodes;
    edge *edges;
};

struct dgraph_t
{
    graph g;
    // dist[node] = int
    GHashTable *dist;
    // parents[node] = node
    GHashTable *parents;
};

#endif
