fn benchmark_osm(osm_path: &Path) {
    // [.. initial setup ..]
    let mut g = Graph::new();
    let mut edges = Vec::new();
    let mut indices = Vec::new();

    // [.. parse nodes and edges ..]

    // Add edges to graph
    for (mut e, (n1, n2)) in edges.into_iter()
        .zip(indices.into_iter()) {
        e.length = graph::haversine_length(
            &g.nodes[g.nodes_idx[&n1]],
            &g.nodes[g.nodes_idx[&n2]]) as u32;
        g.add_edge(n1, n2, e);
    }
    // [.. perform calculations ..]
}
