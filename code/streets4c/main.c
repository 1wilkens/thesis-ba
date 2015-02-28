#include <stdio.h>

#include "osmpbfreader.h"

int main(int argc, char** argv)
{
	printf("This is streets4c\n");

	printf("Opening file %s\n", argv[1]);

	osmpbf_reader_t *reader = osmpbf_init(argv[1]);

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
	printf("Found %d nodes, %d ways and %d relations in %s\n", nodes, ways, relations, argv[1]);

	osmpbf_free(reader);
	return 0;
}
