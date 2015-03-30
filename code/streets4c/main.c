#include <stdio.h>

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

int main(int argc, char** argv)
{
	printf("This is streets4c\n");

	//count_osm(argsv[1]);
	test_graph_structure();

	return 0;
}
