#include "graph.h"

#include <glib.h>

#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "util.h"

#define ADD_ADJ_NODE(n1, n2_idx, edge_idx) g_hash_table_insert(n1->adj, GINT_TO_POINTER(n2_idx), GINT_TO_POINTER(edge_idx))

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

// Dirty hack for printing <.<
static node current;

static void print_adj(gpointer key, gpointer value, gpointer user_data)
{
    graph g = (graph)user_data;
    printf("N%ld -> N%ld [label=%d];\n",
        current->osm_id,
        g->nodes[GPOINTER_TO_INT(key)]->osm_id,
        g->edges[GPOINTER_TO_INT(value)]->driving_time);
}

void print_graph(graph g)
{
    printf("Nodes (%d):\n", g->n_nodes);
    for (int i = 0; i < g->n_nodes; i++)
    {
        node n = g->nodes[i];
        printf("(osm: %ld, id: %d)\n", n->osm_id, N_ID_TO_IDX(g, n->osm_id));
    }
    printf("\nEdges (%d):\n", g->n_edges);
    for (int i = 0; i < g->n_nodes; i++)
    {
        current = g->nodes[i];
        g_hash_table_foreach(current->adj, (GHFunc)print_adj, g);
    }
    printf("\n");
}

graph new_graph(int nodes, int edges)
{
    graph g = malloc(sizeof(struct graph_t));

    g->n_nodes = 0;
    g->n_edges = 0;
    g->nodes = malloc(sizeof(node) * nodes);
    g->edges = malloc(sizeof(edge) * edges);

    g->node_idx = g_hash_table_new(g_direct_hash, g_direct_equal);
    g->edge_idx = g_hash_table_new(g_direct_hash, g_direct_equal);

    return g;
}

void free_graph(graph g)
{
    g_hash_table_destroy(g->node_idx);
    g_hash_table_destroy(g->edge_idx);
    for (int i = 0; i < g->n_edges; i++) { free_edge(g->edges[i]); }
    for (int i = 0; i < g->n_nodes; i++) { free_node(g->nodes[i]); }
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
    if (!g_hash_table_contains(g->node_idx, GINT_TO_POINTER(n1_id))
        || !g_hash_table_contains(g->node_idx, GINT_TO_POINTER(n2_id)))
    {
        free(edge);
        printf("f it: n1=%ld, n2=%ld!\n", n1_id, n2_id);
        return;
    }

    // add edge to graph
    g->edges[g->n_edges] = edge;
    g_hash_table_insert(g->edge_idx, (void*)edge->osm_id, GINT_TO_POINTER(g->n_edges));
    g->n_edges++;

    // link up adjecents
    int edge_idx = g->n_edges - 1;
    int n1_idx = N_ID_TO_IDX(g, n1_id);
    int n2_idx = N_ID_TO_IDX(g, n2_id);

    node n = g->nodes[n1_idx];
    g_hash_table_insert(n->adj, GINT_TO_POINTER(n2_idx), GINT_TO_POINTER(edge_idx));
    ADD_ADJ_NODE(n, n2_idx, edge_idx);

    n = g->nodes[n2_idx];
    ADD_ADJ_NODE(n, n1_idx, edge_idx);
}

dgraph new_dgraph(graph g)
{
    dgraph dg = malloc(sizeof(struct dgraph_t));

    dg->g = g;
    dg->pq = NULL;  // will be initialized in dijkstra(..)

    dg->cur = -1;

    int nn = g->n_nodes;
    dg->dist = malloc(sizeof(int) * nn);
    dg->parents = malloc(sizeof(int) * nn);

    return dg;
}

void free_dgraph(dgraph dg)
{
    free(dg->parents);
    free(dg->dist);
    pqueue_free(dg->pq);
    free(dg);
}

static void relax_edge(gpointer key, gpointer value, gpointer user_data)
{
    //printf("relaxing edges\n");
    dgraph dg = (dgraph)user_data;

    int to_idx = GPOINTER_TO_INT(key);
    edge via = dg->g->edges[GPOINTER_TO_INT(value)];

    int w_cur = dg->dist[to_idx];
    int w_new = dg->dist[dg->cur] + via->driving_time;

    if (w_new == 0 || w_cur == 0)
    {
        printf("that is wrong w_new=%d, w_cur=%d, to_idx=%d, osm_id=%ld\n", w_new, w_cur, to_idx, dg->g->nodes[to_idx]->osm_id);
        int i = *(int*)0;
        printf("%d", i);
    }

    if (w_new > 0 && w_new < w_cur)
    {
        // append new node to queue
        q_elem to = malloc(sizeof(struct q_elem_t));
        to->idx = to_idx;
        to->pri = w_new;
        pqueue_push(dg->pq, to);

        // update to node information in dg
        dg->dist[to_idx] = w_new;
        dg->parents[to_idx] = dg->cur;
    }
}

void dijkstra(dgraph dg, int start)
{
    size_t nn = dg->g->n_nodes;

    // reset distances and parents
    for (size_t i = 0; i < nn; i++)
    {
        dg->dist[i] = INFINITY;
        dg->parents[i] = -1;
    }

    // intialize start node
    dg->dist[start] = 0;

    if (dg->pq)
        pqueue_free(dg->pq);
    dg->pq = pqueue_new(dg->g->n_nodes);
    q_elem s = malloc(sizeof(struct q_elem_t));
    s->idx = start;
    s->pri = 0;
    pqueue_push(dg->pq, s);

    q_elem tmp;
    while((tmp = pqueue_pop(dg->pq)))
    {
        //printf("popped elem\n");
        // relax adjecent edges
        dg->cur = tmp->idx;
        node cur = dg->g->nodes[dg->cur];
        g_hash_table_foreach(cur->adj, (GHFunc)relax_edge, dg);
        free(tmp);
    }
}

double haversine_length(node n1, node n2) {
    double lat1 = RAD(n1->lat);
    double lon1 = RAD(n1->lon);
    double lat2 = RAD(n2->lat);
    double lon2 = RAD(n2->lon);
    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;
    double a = pow(sin(dlat/2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlon/2), 2);
    double c = 2 * asin(sqrt(a));
    return 6367000 * c;  // distance in m
}
