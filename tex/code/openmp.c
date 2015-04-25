static void benchmark_osm(char* path)
{
    // [.. parse input data ..]

    // Setup OpenMP
    omp_set_dynamic(0);
    omp_set_num_threads(NUM_THREADS);

    #pragma omp parallel
    {
        // Calculate limits for this thread
        int id = omp_get_thread_num();
        int first = id * nodes_per_thread;
        int last = first + nodes_per_thread;

        dgraph dg = new_dgraph(g);
        for (int i = first; i < last; i++)
        {
            dijkstra(dg, i); // Calculate shortest paths
        }
        free_dgraph(dg);
    }

    // [.. final cleanup ..]
}
