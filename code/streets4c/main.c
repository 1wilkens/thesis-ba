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
				printf("w.n_refs: %d\n", w->n_refs);
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
	int n_nodes = 0, n_ways = 0;

	osmpbf_reader_t *reader = osmpbf_init(path);
	OSMPBF__PrimitiveBlock *pb;
	while((pb = get_next_primitive(reader)) != NULL)
	{
		for (size_t i = 0; i < pb->n_primitivegroup; i++)
		{
			n_nodes += pb->primitivegroup[i]->n_nodes;
			if (pb->primitivegroup[i]->dense != NULL)
			{
				n_nodes += pb->primitivegroup[i]->dense->n_id;
			}
			n_ways += pb->primitivegroup[i]->n_ways;
		}

		osmpbf__primitive_block__free_unpacked(pb, NULL);
	}
	osmpbf_free(reader);

	graph g = new_graph(n_nodes, n_ways);
	edge *edges = malloc(sizeof(*edges) * n_ways);
	long *n1 = malloc(sizeof(*n1) * n_ways);
	long *n2 = malloc(sizeof(*n2) * n_ways);
	n_ways = 0;

	reader = osmpbf_init(path);
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
				add_node(g, n);
			}
			if (pg->dense != NULL)
			{
				OSMPBF__DenseNodes *dn = pg->dense;
				for (size_t k = 0; k < dn->n_id; k++)
				{
					// todo: handle densenodes
					// node n = new_node();
					// n->osm_id = pg->nodes[i]->id;
					// n->lat = pg->nodes[i]->lat;
					// n->lon = pg->nodes[i]->lon;
					// add_node(g, n);
				}
			}
			for (size_t j = 0; j < pg->n_ways; j++)
			{
				edges[n_ways] = new_edge();
				edges[n_ways]->osm_id = pg->ways[j]->id;

				long ref = 0;
				for (size_t k = 0; k < pg->ways[j]->n_refs; k++)
				{

				}
				n_ways++;
			}
		}

		osmpbf__primitive_block__free_unpacked(pb, NULL);
	}
	osmpbf_free(reader);

	free(n1);
	free(n2);
	free(edges);

	free_graph(g);
}

int main(int argc, char** argv)
{
	printf("This is streets4c\n");

	count_osm(argv[1]);
	//test_graph_structure();
	//benchmark_osm(argv[1]);

	return 0;
}
