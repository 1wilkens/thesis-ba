/* somewhere in a function */
osmpbf_reader_t *reader = osmpbf_init(<some_path>);

OSMPBF__PrimitiveBlock *pb;
while((pb = get_next_primitive(reader)) != NULL)
{
    for (size_t i = 0; i < pb->n_primitivegroup; i++)
    {
        // access data on the primitive groups
        OSMPBF__PrimitiveGroup *pg = pb->primitivegroup[i];

        /* no need to free pg here since its part
         * of the primitive block pb */
    }

    // cannot use free(pb) here because of Protobuf
    osmpbf__primitive_block__free_unpacked(pb, NULL);
}

// regular free function provided by library
osmpbf_free(reader);
/* remaining part of the function */
