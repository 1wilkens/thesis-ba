fn benchmark_osm(osm_path: &Path) {
    // [.. parse input data ..]
    let graph = &g; // immutable reference to copy into the closures below

    let num_nodes = NUM_NODES / NUM_THREADS;
    let mut guards = Vec::with_capacity(NUM_THREADS);

    // Spawn NUM_THREADS amount of worker threads
    for id in 0..NUM_THREADS {
        guards.push(thread::scoped(move || {
            let first = id * num_nodes;
            let last = first + num_nodes;
            let mut dg = DijkstraGraph::from_graph(graph);

            for n in first..last {
                dg.dijkstra(n);
            }
        }));
    }

    // Join all threads before exiting
    for g in guards {
        g.join();
    }
}
