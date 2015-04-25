func benchmarkOsm(path string) {
    // [.. parse input data ..]

    // To wait goroutines later on
    var wg sync.WaitGroup
    runtime.GOMAXPROCS(NUM_THREADS)

    for i := 0; i < NUM_THREADS; i++ {
        wg.Add(1)   // Increase WaitGroup counter
        go func(id int) {
            defer wg.Done() // Decrease counter when the goroutine exits

            dg := FromGraph(g)
            first := id * nodes_per_thread
            last := first + nodes_per_thread

            for n := first; n < last; n++ {
                dg.Dijkstra(n)
            }
        }(i)
    }

    wg.Wait()
}
