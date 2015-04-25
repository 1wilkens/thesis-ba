pub struct Node {
    pub osm_id: i64,
    pub lon: f64,
    pub lat: f64,

    pub adj: HashMap<usize, usize> // == adjecent edges/nodes
}

pub struct Edge {
    pub osm_id: i64,
    pub length: u32, // == edge weight
    pub max_speed: u8,
    pub driving_time: u32
}

pub struct Graph {
    pub nodes: Vec<Node>,
    pub edges: Vec<Edge>,

    pub nodes_idx: HashMap<i64, usize>,
    pub edges_idx: HashMap<i64, usize>
}

pub struct DijkstraGraph<'a> {
    pub graph: &'a Graph,
    pub queue: BinaryHeap<NodeState>,
    pub dist: Vec<u32>,
    pub parents: Vec<usize>
}
