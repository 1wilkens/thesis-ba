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

//#define CALC_LAT(lat, pb) (exp(-9) * (pb->lat_offset + (pb->granularity * lat)))
//#define CALC_LON(lon, pb) (exp(-9) * (pb->lon_offset + (pb->granularity * lon)))

#define CALC_LAT(lat, pb) lat/10000000.00   // What the actual hell? above formula should be correct..
#define CALC_LON(lon, pb) lon/10000000.00

#endif
