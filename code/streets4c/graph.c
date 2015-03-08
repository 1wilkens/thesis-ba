#include "graph.h"

#include <glib.h>
#include <limits.h>
#include <stdlib.h>

#include "util.h"

#define INFINITY INT_MAX;

node new_node()
{
    node n = malloc(sizeof(struct node_t));
    n->adj = g_hash_table_new(g_direct_hash, g_direct_equal);

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
    g->nodes[g->n_nodes++] = n;
}
void add_edge(graph g, node n1, node n2, edge e)
{
    node n;

    int done = 0;
    for (size_t i = 0; i < (size_t)g->n_nodes && done < 2; i++)
    {
        n = g->nodes[i];
        if (n == n1)
        {
            g_hash_table_insert(n1->adj, n2, e);
            done++;
        } else if (n == n2)
        {
            g_hash_table_insert(n2->adj, n1, e);
            done++;
        }
    }
}

dgraph dgraph_new(graph g)
{
    dgraph dg = malloc(sizeof(dgraph));

    int nn = g->n_nodes;

    dg->g = g;
    dg->pq = pqueue_new(nn);

    dg->cur = NULL;

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

    node to = (node)key;
    edge via = (edge)value;

    int w_cur = dg->dist[GPOINTER_TO_INT(g_hash_table_lookup(dg->g->node_idx, to))];
    int w_new = dg->dist[GPOINTER_TO_INT(g_hash_table_lookup(dg->g->node_idx, dg->cur))] + via->driving_time;

    if (w_new < w_cur)
    {
        // append new node to queue
        pqueue_push(dg->pq, to, w_new);
        // set parent[to] = cur
        dg->parents[GPOINTER_TO_INT(g_hash_table_lookup(dg->g->node_idx, to))] = dg->cur;
    }
}

void dijkstra(dgraph dg, node start)
{
    size_t nn = dg->g->n_nodes;

    // reset distances and parents
    for (size_t i = 0; i < nn; i++)
    {
        dg->dist[i] = INFINITY;
        dg->parents[i] = NULL;
    }

    // intialize start node
    dg->dist[GPOINTER_TO_INT(g_hash_table_lookup(dg->g->node_idx, start))] = 0;

    //dq->pq = pqueue_new(dg->g->n_nodes);
    pqueue_push(dg->pq, start, 0);

    while((dg->cur = pqueue_pop(dg->pq)))
    {
        // relax adjecent edges
        g_hash_table_foreach(dg->cur->adj, (GHFunc)relax_edge, dg);
    }
}
