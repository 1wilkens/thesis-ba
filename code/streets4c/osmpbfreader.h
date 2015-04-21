#ifndef OSMPBFREADER_H
#define OSMPBFREADER_H

#include <math.h>
#include <stdio.h>

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

#define CALC_LAT(lat, pb) lat/10000000.00   // Testing verified these are correct..
#define CALC_LON(lon, pb) lon/10000000.00

#endif
