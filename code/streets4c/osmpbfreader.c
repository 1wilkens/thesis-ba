#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>

#include <osmpbf/osmpbf.h>
#include <zlib.h>

#include "osmpbfreader.h"

static OSMPBF__BlobHeader *read_blob_header(osmpbf_reader_t*);
static int read_blob(osmpbf_reader_t*, OSMPBF__BlobHeader*);

osmpbf_reader_t *osmpbf_init(const char *file)
{
	osmpbf_reader_t *r = malloc(sizeof(osmpbf_reader_t));

	r->buf = calloc(max_uncompressed_blob_size, 1);
	r->unpack_buf = calloc(max_uncompressed_blob_size, 1);
	r->file = fopen(file, "rb");

	return r;
}

void osmpbf_free(osmpbf_reader_t *r)
{
	fclose(r->file);
	free(r->buf);
	free(r->unpack_buf);
	free(r);
}

OSMPBF__PrimitiveBlock *get_next_primitive(osmpbf_reader_t *r)
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

static OSMPBF__BlobHeader *read_blob_header(osmpbf_reader_t *r)
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

static int read_blob(osmpbf_reader_t *r, OSMPBF__BlobHeader *h)
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
