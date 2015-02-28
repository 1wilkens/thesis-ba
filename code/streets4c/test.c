#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "graph.h"

int main(int argc, char** argv)
{
    srand(time(NULL));
    queue_t *q = new_queue(50);

    for (int i = 0; i < 50; i++)
    {
        node_t *n = malloc(sizeof(node_t));
        n->cost = rand() % 1000;
        queue_push(q, n);
    }

    for (int i = 0; i < 50; i++)
    {
        node_t *n = queue_pop(q);
        printf("Got node #%d with cost=%d\n", i, n->cost);
        free(n);
    }

    return 0;
}
