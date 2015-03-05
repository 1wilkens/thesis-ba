#include "graph.h"

#include <stdlib.h>

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

    return g;
}

void free_graph(graph g)
{
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
    for (size_t i = 0; i < (size_t)g->n_nodes &&  done < 2; i++)
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
    dg->g = g;
    dg->dist = g_hash_table_new(g_direct_hash, g_direct_equal);
    dg->parents = g_hash_table_new(g_direct_hash, g_direct_equal);

    return dg;
}

void dgraph_free(dgraph dg)
{
    g_hash_table_destroy(dg->parents);
    g_hash_table_destroy(dg->dist);
    free(dg);
}

void dijkstra(dgraph dg, node n);
