pub fn add_edge(&mut self, n1: i64, n2: i64, e: Edge) {
    // [..]
    // link up adjecents
    let n1_idx = self.nodes_idx.get(&n1).unwrap();
    let n2_idx = self.nodes_idx.get(&n2).unwrap();
    // [..]
}
