extern crate osmpbfreader;

#[allow(dead_code)]
mod graph;

use std::env;
use std::collections::hash_map::HashMap;
use std::fs::File;
use std::path::Path;

use osmpbfreader::{OsmPbfReader, blocks, objects};

use graph::{DijkstraGraph, Edge, Graph, Node, INFINITY};

const NODES: i64 = 10;
const EDGES: i64 = 16;
const MAX_WEIGTH: i64 = 14;

fn main() {
    println!("This is streets4rust\n");

    //let args: Vec<String> = env::args().collect();
    //count_osm(Path::new(&args[1]));
    test_graph_structure();
}

fn count_osm(osm_path: &Path) {
    println!("Opening osm file: {}", osm_path.display());
    let osm_file = File::open(osm_path).unwrap();
    let mut pbf_reader = OsmPbfReader::with_reader(osm_file);

    let mut nodes = 0u64;
    let mut ways  = 0u64;
    let mut rels  = 0u64;

    for block in pbf_reader.primitive_blocks().map(|b| b.unwrap()) {
        for obj in blocks::iter(&block) {
            match obj {
                objects::OsmObj::Node(_)        => nodes += 1,
                objects::OsmObj::Way(_)         => ways += 1,
                objects::OsmObj::Relation(_)    => rels += 1
                //_                                   => println!("Found unknown object")
            }
        }
    }

    println!("Found {} nodes, {} ways and {} relations in {}", nodes, ways, rels, osm_path.display());
}

fn test_graph_structure() {
    let mut g = Graph::new();
	for i in 1i64..NODES+1 {
		let n = Node{ osm_id: i * 5, lat: 0f64, lon: 0f64, adj: HashMap::new() };
		g.add_node(n);
	}
	for i in 0i64..(EDGES/2) {
		let (n1, n2, n3): (i64, i64, i64) = (i * 5, (i + 2) * 5, (i + 5) * 5);
		let e = Edge{ osm_id: i, length: 0, max_speed: 0, driving_time: ((i % MAX_WEIGTH) + 1) as u32 };
		g.add_edge(n1, n2, e);
		let e = Edge{ osm_id: i + EDGES, length: 0, max_speed: 0, driving_time: ((i + 5%MAX_WEIGTH) + 1) as u32 };
		g.add_edge(n1, n3, e);
	}

	g.print();
	let mut dg = DijkstraGraph::from_graph(&g);

	dg.dijkstra(0);
	let s = &dg.graph.nodes[0];

	for i in 1..10 {
        let mut p = &dg.graph.nodes[i];
		if dg.dist[i] == INFINITY {
			println!("Skipping {} since its unreachable!", p.osm_id);
			continue;
		}
		println!("Started @ {} with distance: {}", p.osm_id, dg.dist[i]);
		while s.osm_id != p.osm_id {
			p = &dg.graph.nodes[dg.parents[dg.graph.nodes_idx[&p.osm_id]]];
			println!("Discovered {}", p.osm_id);
		}
		println!("Finished!");
	}
}
