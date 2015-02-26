#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>

#include <osmpbf/osmpbf.h>
#include <zlib.h>

typedef struct
{
	FILE *file;
	uint8_t *buf;
	uint8_t *unpack_buf;
} osmpbf_reader;

static osmpbf_reader *osmpbf_init(const char *file)
{
	osmpbf_reader *r = malloc(sizeof(osmpbf_reader));

	r->buf = calloc(max_uncompressed_blob_size, 1);
	r->unpack_buf = calloc(max_uncompressed_blob_size, 1);
	r->file = fopen(file, "rb");

	return r;
}

static void osmpbf_free(osmpbf_reader *r)
{
	fclose(r->file);
	free(r->buf);
	free(r->unpack_buf);
	free(r);
}

static OSMPBF__BlobHeader *read_blob_header(osmpbf_reader *r)
{
	// read BlobHeader size
	int sz = 0;
	if (fread(&sz, 1, 4, r->file) == EOF)
	{
		return NULL;
	}
	sz = ntohl(sz);
	// read sz amount of bytes and unpack the header
	fread(r->buf, 1, sz, r->file);
	return osmpbf__blob_header__unpack(NULL, sz, r->buf);
}

static int read_blob(osmpbf_reader *r, OSMPBF__BlobHeader *h)
{
	// read blob from file with size h->datasize
	int sz = h->datasize;
	fread(r->buf, 1, sz, r->file);
	OSMPBF__Blob *b = osmpbf__blob__unpack(NULL, sz, r->buf);
	if (b->has_raw)
	{
		// Blob has raw data
		sz = sizeof(b->raw);
		memcpy(r->unpack_buf, r->buf, sz);
	}
	else
	{
		// Assume the data is compressed with zlib
		sz = b->zlib_data.len;

		z_stream z;
		z.next_in   = b->zlib_data.data;
	    z.avail_in  = sz;
	    z.next_out  = (unsigned char*) r->unpack_buf;
	    z.avail_out = b->raw_size;
	    z.zalloc    = Z_NULL;
	    z.zfree     = Z_NULL;
	    z.opaque    = Z_NULL;
		inflateInit(&z);
		if (inflate(&z, Z_FINISH) != Z_STREAM_END)
		{
			printf("inflate != Z_STREAM_END\n");
		}
		inflateEnd(&z);
		sz = z.total_out;
	}

	osmpbf__blob__free_unpacked(b, NULL);
	return sz;
}

static OSMPBF__PrimitiveBlock *get_next_primitive(osmpbf_reader *r)
{
	OSMPBF__BlobHeader *h = NULL;
	do
	{
		if (h != NULL)
		{
			// free previous header
			osmpbf__blob_header__free_unpacked(h, NULL);
		}

		h = read_blob_header(r);
		if (h == NULL)
		{
			// Reached EOF
			return NULL;
		}
		int sz = read_blob(r, h);
		if (strcmp(h->type, "OSMData") == 0)
		{
			osmpbf__blob_header__free_unpacked(h, NULL);
			return osmpbf__primitive_block__unpack(NULL, sz, r->unpack_buf);
		}
	} while (h == NULL || strcmp(h->type, "OSMHeader") == 0);

	return NULL;
}

int main(int argc, char** argv)
{
	printf("This is streets4c\n");

	printf("Opening file %s\n", argv[1]);

	osmpbf_reader *reader = osmpbf_init(argv[1]);

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
