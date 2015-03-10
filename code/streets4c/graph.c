#include "graph.h"

#include <glib.h>
#include <limits.h>
#include <stdlib.h>

#include "util.h"

#define INFINITY INT_MAX;

#define N_ID_TO_IDX(g, id) GPOINTER_TO_INT(g_hash_table_lookup(g->node_idx, (void*)id))
#define E_ID_TO_IDX(g, id) GPOINTER_TO_INT(g_hash_table_lookup(g->edge_idx, (void*)id))

#define ADD_ADJ_NODE(n1, n2_idx, edge_idx) g_hash_table_insert(n1->adj, GINT_TO_POINTER(n2_idx), GINT_TO_POINTER(edge_idx))

node new_node()
{
    node n = malloc(sizeof(struct node_t));
    n->adj = g_hash_table_new(g_int_hash, g_int_equal);

    return n;
}

void free_node(node n)
{
    g_hash_table_destroy(n->adj);
    free(n);
}

edge new_edge()
{
    return malloc(sizeof(struct edge_t));
}

void free_edge(edge e)
{
    free(e);
}

graph new_graph(int cap)
{
    graph g = malloc(sizeof(struct graph_t));

    g->n_nodes = 0;
    g->n_edges = 0;
    g->nodes = malloc(sizeof(node) * cap);
    g->edges = malloc(sizeof(edge) * cap);

    g->node_idx = g_hash_table_new(g_direct_hash, g_direct_equal);
    g->edge_idx = g_hash_table_new(g_direct_hash, g_direct_equal);

    return g;
}

void free_graph(graph g)
{
    g_hash_table_destroy(g->node_idx);
    g_hash_table_destroy(g->edge_idx);
    free(g->edges);
    free(g->nodes);
    free(g);
}

void add_node(graph g, node n)
{
    g->nodes[g->n_nodes] = n;
    g_hash_table_insert(g->node_idx, (void*)n->osm_id, GINT_TO_POINTER(g->n_nodes));
    g->n_nodes++;
}

void add_edge(graph g, long n1_id, long n2_id, edge edge)
{
    // add edge to graph
    g->edges[g->n_edges] = edge;
    g_hash_table_insert(g->edge_idx, (void*)edge->osm_id, GINT_TO_POINTER(g->n_edges));
    g->n_edges++;

    // link up adjecents
    int edge_idx = g->n_edges - 1;
    int n1_idx = N_ID_TO_IDX(g, n1_id);
    int n2_idx = N_ID_TO_IDX(g, n2_id);

    node n = g->nodes[n1_idx];
    ADD_ADJ_NODE(n, n2_idx, edge_idx);

    n = g->nodes[n2_idx];
    ADD_ADJ_NODE(n, n1_idx, edge_idx);
}

dgraph dgraph_new(graph g)
{
    dgraph dg = malloc(sizeof(dgraph));

    dg->g = g;
    dg->pq = NULL;  // will be initialized in dijkstra(..)

    dg->cur = -1;

    int nn = g->n_nodes;
    dg->dist = malloc(sizeof(int) * nn);
    dg->parents = malloc(sizeof(node*) * nn);

    return dg;
}

void dgraph_free(dgraph dg)
{
    free(dg->parents);
    free(dg->dist);
    pqueue_free(dg->pq);
    free(dg);
}

static void relax_edge(gpointer key, gpointer value, gpointer user_data)
{
    dgraph dg = (dgraph)user_data;

    int to_idx = GPOINTER_TO_INT(key);
    edge via = dg->g->edges[GPOINTER_TO_INT(value)];

    int w_cur = dg->dist[to_idx];
    int w_new = dg->dist[dg->cur] + via->driving_time;

    if (w_new < w_cur)
    {
        // append new node to queue
        pqueue_push(dg->pq, GINT_TO_POINTER(to_idx), w_new);
        // set parent[to] = cur
        dg->parents[to_idx] = dg->cur;
    }
}

void dijkstra(dgraph dg, node start)
{
    size_t nn = dg->g->n_nodes;

    // reset distances and parents
    for (size_t i = 0; i < nn; i++)
    {
        dg->dist[i] = INFINITY;
        dg->parents[i] = -1;
    }

    // intialize start node
    int start_idx = GPOINTER_TO_INT(g_hash_table_lookup(dg->g->node_idx, start));
    dg->dist[start_idx] = 0;

    if (dg->pq)
        pqueue_free(dg->pq);
    dg->pq = pqueue_new(dg->g->n_nodes);
    pqueue_push(dg->pq, GINT_TO_POINTER(start_idx), 0);

    while((dg->cur = GPOINTER_TO_INT(pqueue_pop(dg->pq))))
    {
        // relax adjecent edges
        node cur = dg->g->nodes[dg->cur];
        g_hash_table_foreach(cur->adj, (GHFunc)relax_edge, dg);
    }
}
