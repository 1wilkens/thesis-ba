struct node_t
{
    long osm_id;
    double lon, lat;

    GHashTable *adj; // == adjecent edges/nodes
};

struct edge_t
{
    long osm_id;
    int length;
    int max_speed;
    int driving_time; // == edge weight
};

struct graph_t
{
    int n_nodes, n_edges;
    node *nodes;
    edge *edges;

    GHashTable *node_idx;
    GHashTable *edge_idx;
};

struct dgraph_t
{
    graph g;
    pqueue pq;

    int cur; // == index of current node to explore
    int *dist;
    int *parents;
};
