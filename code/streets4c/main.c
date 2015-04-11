#include <stdio.h>
#include <stdlib.h>

#include "osmpbfreader.h"
#include "graph.h"

#define NODES 10
#define EDGES 16

#define MAX_WEIGTH 14

static void count_osm(char* path)
{
	printf("Opening file %s\n", path);
	osmpbf_reader_t *reader = osmpbf_init(path);

	int nodes = 0, ways = 0, relations = 0;

	OSMPBF__PrimitiveBlock *pb;
	while((pb = get_next_primitive(reader)) != NULL)
	{
		for (size_t i = 0; i < pb->n_primitivegroup; i++)
		{
			OSMPBF__PrimitiveGroup *pg = pb->primitivegroup[i];
			for (size_t j = 0; j < pg->n_nodes; j++)
			{
				OSMPBF__Node *n = pg->nodes[j];
				nodes++;
			}
			if (pg->dense != NULL)
			{
				OSMPBF__DenseNodes *dn = pg->dense;
				for (size_t k = 0; k < dn->n_id; k++)
				{
					nodes++;
				}
			}
			for (size_t j = 0; j < pg->n_ways; j++)
			{
				OSMPBF__Way *w = pg->ways[j];
				ways++;
			}
			for (size_t j = 0; j < pg->n_relations; j++)
			{
				OSMPBF__Relation *r = pg->relations[i];
				relations++;
			}
		}

		osmpbf__primitive_block__free_unpacked(pb, NULL);
	}
	printf("Found %d nodes, %d ways and %d relations in %s\n", nodes, ways, relations, path);

	osmpbf_free(reader);
}

static void test_graph_structure(void)
{
	graph g = new_graph(NODES, EDGES);
	for (int i = 1; i <= NODES; i++)
	{
		node n = new_node();
		n->osm_id = i * 5;
		add_node(g, n);
	}
	for (int i = 0; i < EDGES/2; i++)
	{
		int n1 = i * 5;
		int n2 = (i + 2) * 5;
		int n3 = (i + 5) * 5;
		edge e = new_edge();
		e->osm_id = i;
		e->driving_time = (i % MAX_WEIGTH) + 1;
		add_edge(g, n1, n2, e);
		e = new_edge();
		e->osm_id = i + EDGES;
		e->driving_time = (i + 5%MAX_WEIGTH) + 1;
		add_edge(g, n1, n3, e);
	}

	print_graph(g);
	dgraph dg = new_dgraph(g);

	dijkstra(dg, 0);
	node s = dg->g->nodes[0];

	for (int i = 1; i < 10; i++)
	{
		node p = dg->g->nodes[i];
		if (dg->dist[i] == INFINITY)
		{
			printf("Skipping %ld since its unreachable!\n", p->osm_id);
			continue;
		}
		printf("Started @ %ld with distance: %d\n", p->osm_id, dg->dist[i]);
		while (s != p)
		{
			p = dg->g->nodes[dg->parents[GPOINTER_TO_INT(g_hash_table_lookup(dg->g->node_idx, (void*)p->osm_id))]];
			printf("Discovered %ld\n", p->osm_id);
		}
		printf("Finished!\n");
	}

	free_dgraph(dg);
	free_graph(g);
}

static void benchmark_osm(char* path)
{
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
				long id = 0;
				float lat = 0.0, lon = 0.0; // DenseNodes are delta coded
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
				long id = 0;	// refs are delta coded
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
	g_ptr_array_free(nodes, TRUE);
	printf("Parsed and added %d nodes..\n", g->n_nodes);

	int c = 0;
	for (size_t i = 0; i < edges->len; i++)
	{
		// add edges
		long n_1 = g_array_index(n1, long, i);
		long n_2 = g_array_index(n2, long, i);
		edge e = g_ptr_array_index(edges, i);
		e->length = haversine_length(g->nodes[N_ID_TO_IDX(g, n_1)], g->nodes[N_ID_TO_IDX(g, n_2)]);
		if (e->length == 0)
			printf("length %d\n", c++);
		add_edge(g, n_1, n_2, e);
	}
	g_array_free(n1, TRUE);
	g_array_free(n2, TRUE);
	g_ptr_array_free(edges, TRUE);
	printf("Added %d edges..\n", g->n_edges);

	printf("Calculating shortest paths for all nodes..\n");
	dgraph dg = new_dgraph(g);
	for (size_t i = 0; i < g->n_nodes; i++)
	{
		printf("Starting node #%d\n", i);
		dijkstra(dg, i);
		printf("Finished node #%d\n", i);
		if (i+1%100000 == 0)
		{
			printf("Finished node #%d\n", i);
		}
	}
	free_dgraph(dg);
	free_graph(g);
}

int main(int argc, char** argv)
{
	printf("This is streets4c\n");

	//count_osm(argv[1]);
	//test_graph_structure();
	benchmark_osm(argv[1]);

	return 0;
}
