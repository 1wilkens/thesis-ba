#ifndef OSMPBFREADER_H
#define OSMPBFREADER_H

#include <stdio.h>

#include <osmpbf/osmpbf.h>
#include <osmpbf/fileformat.pb-c.h>
#include <osmpbf/osmformat.pb-c.h>

typedef struct osmpbf_reader_t osmpbf_reader_t;

struct osmpbf_reader_t
{
	FILE *file;
	uint8_t *buf;
	uint8_t *unpack_buf;
};

osmpbf_reader_t *osmpbf_init(const char*);
void osmpbf_free(osmpbf_reader_t*);

OSMPBF__PrimitiveBlock *get_next_primitive(osmpbf_reader_t *r);

#endif
