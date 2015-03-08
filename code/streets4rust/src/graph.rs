use std::collections::hash_map::HashMap;
use std::collections::binary_heap::BinaryHeap;

struct Node {
    pub osm_id: i64,

    pub lon: f64,
    pub lat: f64,

    pub adj: HashMap<usize, usize>
}

#[derive(PartialEq, Eq, Hash)]
struct Edge {
    pub osm_id: i64,

    pub length: u32,
    pub max_speed: u8,
    pub driving_time: u32
}

struct Graph {
    pub nodes: Vec<Node>,
    pub edges: Vec<Edge>,

    pub nodes_idx: HashMap<i64, usize>,
    pub edges_idx: HashMap<i64, usize>
}

impl Graph {
    pub fn new() -> Graph {
        Graph {
            nodes: Vec::new(),
            edges: Vec::new(),

            nodes_idx: HashMap::new(),
            edges_idx: HashMap::new()
        }
    }
}

struct DijkstraGraph<'a> {
    pub graph: &'a Graph,

    pub queue: BinaryHeap<NodeState>,

    pub dist: Vec<u32>,
    pub parents: Vec<usize>
}

impl <'a> DijkstraGraph<'a> {
    pub fn from_graph(graph: &Graph) -> DijkstraGraph {
        DijkstraGraph {
            graph: graph,

            queue: BinaryHeap::new(),

            dist: Vec::with_capacity(graph.nodes.len()),
            parents: Vec::with_capacity(graph.nodes.len())
        }
    }

    pub fn dijkstra(&mut self, start_node: usize) {
        for i in 0..self.graph.nodes.len() {
            self.dist[i] = ::std::u32::MAX;
            self.parents[i] = -1;
        }

        self.queue.clear(); // just to be sure
        self.queue.push(NodeState { idx: start_node, dist: 0});

        while let Some(NodeState { idx, dist }) = self.queue.pop() {
            // to skip node with already better results
            if dist <= self.dist[idx] {
                let cur = &self.graph.nodes[idx];

                // relax all adjecent edges
                for (edge, node) in cur.adj.iter() {
                    let w_cur = self.dist[*node];
                    let w_new = self.dist[idx] + self.graph.edges[*edge].driving_time;

                    if w_new < w_cur {
                        self.dist[*node] = w_new;
                        self.parents[*node] = idx;
                        self.queue.push(NodeState { idx: *node, dist: w_new })
                    }
                }
            }
        }
    }
}

#[derive(Copy, Eq, PartialEq)]
struct NodeState {
    idx: usize,
    dist: u32
}

impl Ord for NodeState {
    fn cmp(&self, other: &NodeState) -> ::std::cmp::Ordering {
        // reverse ordering to get min heap behaviour
        other.dist.cmp(&self.dist)
    }
}

impl PartialOrd for NodeState {
    fn partial_cmp(&self, other: &NodeState) -> Option<::std::cmp::Ordering> {
        Some(self.cmp(other))
    }
}
