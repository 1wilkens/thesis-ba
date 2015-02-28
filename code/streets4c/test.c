#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "util.h"

int main(int argc, char** argv)
{
    srand(time(NULL));
    hashtable_t *h = hashtable_new(100);

    for (int i = 0; i < 100; i++)
    {
        long key = rand();
        char* str = malloc(15);
        sprintf(str, "val:%d", key);
        hash_insert(h, key, str);
    }

    for (int i = 0; i < 100; i++)
    {
        printf("key=%d, value=%s\n", h->keys[i], h->values[i]);
        free(h->values[i]);

    }
    
    hashtable_free(h);
    return 0;
}
