#include <stdio.h>
#include <stdlib.h>

#include <omp.h>

#include "osmpbfreader.h"
#include "graph.h"

#define NUM_NODES 100000
#define NUM_THREADS 4

int main(int argc, char** argv)
{
    char *path = argv[1];
    int num_threads = argc > 2 ? atoi(argv[2]) : NUM_THREADS;
    printf("This is streets4c\n");
    printf("Starting with NUM_THREADS=%d, NUM_NODES=%d\n", num_threads, NUM_NODES);
    printf("Opening file %s\n", path);

    GPtrArray *nodes = g_ptr_array_new();
    GPtrArray *edges = g_ptr_array_new();
    GArray *n1 = g_array_new(TRUE, TRUE, sizeof(long));
    GArray *n2 = g_array_new(TRUE, TRUE, sizeof(long));

    osmpbf_reader_t *reader = osmpbf_init(path);
    OSMPBF__PrimitiveBlock *pb;
    while((pb = get_next_primitive(reader)) != NULL)
    {
        for (size_t i = 0; i < pb->n_primitivegroup; i++)
        {
            OSMPBF__PrimitiveGroup *pg = pb->primitivegroup[i];
            for (size_t j = 0; j < pg->n_nodes; j++)
            {
                node n = new_node();
                n->osm_id = pg->nodes[i]->id;
                n->lat = CALC_LAT(pg->nodes[i]->lat, pb);
                n->lon = CALC_LON(pg->nodes[i]->lon, pb);
                g_ptr_array_add(nodes, n);
            }
            if (pg->dense != NULL)
            {
                OSMPBF__DenseNodes *dn = pg->dense;
                long id = 0, lat = 0.0, lon = 0.0; // DenseNodes are delta coded
                for (size_t k = 0; k < dn->n_id; k++)
                {
                    id += dn->id[k];
                    lat += dn->lat[k];
                    lon += dn->lon[k];

                    node n = new_node();
                    n->osm_id = id;
                    n->lat = CALC_LAT(lat, pb);
                    n->lon = CALC_LON(lon, pb);
                    g_ptr_array_add(nodes, n);
                }
            }
            for (size_t j = 0; j < pg->n_ways; j++)
            {
                long id = 0;    // refs are delta coded
                for (size_t k = 0; k < pg->ways[j]->n_refs - 1; k++)
                {
                    id += pg->ways[j]->refs[k];
                    long id2 = id + pg->ways[j]->refs[k+1]; // need variable for append_val
                    g_array_append_val(n1, id);
                    g_array_append_val(n2, id2);

                    edge e = new_edge();
                    e->osm_id = pg->ways[j]->id;
                    g_ptr_array_add(edges, e);
                }
            }
        }
        osmpbf__primitive_block__free_unpacked(pb, NULL);
    }
    osmpbf_free(reader);

    graph g = new_graph(nodes->len, edges->len);
    for (size_t i = 0; i < nodes->len; i++)
    {
        // add nodes
        add_node(g, g_ptr_array_index(nodes, i));
    }
    g_ptr_array_free(nodes, TRUE);  // free as earl as possible
    printf("Parsed and added %d nodes..\n", g->n_nodes);

    for (size_t i = 0; i < edges->len; i++)
    {
        // add edges
        long n_1 = g_array_index(n1, long, i);
        long n_2 = g_array_index(n2, long, i);
        edge e = g_ptr_array_index(edges, i);
        e->length = haversine_length(g->nodes[N_ID_TO_IDX(g, n_1)], g->nodes[N_ID_TO_IDX(g, n_2)]);
        add_edge(g, n_1, n_2, e);
    }
    g_array_free(n1, TRUE);
    g_array_free(n2, TRUE);
    g_ptr_array_free(edges, TRUE);
    printf("Added %d edges..\n", g->n_edges);

    printf("Calculating shortest paths for the first %d nodes..\n", NUM_NODES);
    int nodes_per_thread = NUM_NODES / num_threads;

    // Setup OpenMP
    omp_set_dynamic(0);
    omp_set_num_threads(num_threads);

    #pragma omp parallel    // This block runs in each of the NUM_THREADS worker threads
    {
        int id = omp_get_thread_num();
        int first = id * nodes_per_thread;
        int last = first + nodes_per_thread;

        dgraph dg = new_dgraph(g);
        printf("[Thread #%d] Starting calculation from %d to %d\n", id, first, last);
        for (int i = first; i < last; i++)
        {
            dijkstra(dg, i);
            if ((i+1)%1000 == 0)
            {
                printf("[Thread #%d] Finished node #%d\n", id, i+1);
            }
        }
        free_dgraph(dg);
    }   // End of the block inplicitly joins all threads
    free_graph(g);

    printf("Finished all calculations!\n");
    return EXIT_SUCCESS;
}
