pub fn add_edge(&mut self, n1_id: i64, n2_id: i64, e: Edge) {
    // [..]
    // link up adjecents
    let n1_idx = self.nodes_idx.get(&n1_id).unwrap();
    let n2_idx = self.nodes_idx.get(&n2_id).unwrap();
    // [..]
}
