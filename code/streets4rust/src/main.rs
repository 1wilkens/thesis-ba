#![feature(scoped)]

extern crate osmpbfreader;

#[allow(dead_code)]
mod graph;

use std::env;
use std::collections::hash_map::HashMap;
use std::fs::File;
use std::path::Path;
use std::str::FromStr;
use std::thread;

use osmpbfreader::{OsmPbfReader, blocks, objects};

use graph::{DijkstraGraph, Edge, Graph, Node};

const NUM_NODES: usize = 100_000;
const NUM_THREADS: usize = 4;

fn main() {
    let args: Vec<String> = env::args().collect();
    let path = Path::new(&args[1]);
    let num_threads = if args.len() > 2 { usize::from_str(&args[2]).unwrap() } else { NUM_THREADS };

    println!("This is streets4rust");
    println!("Starting with NUM_THREADS={}, NUM_NODES={}", num_threads, NUM_NODES);
    println!("Opening osm file: {}", path.display());
    let osm_file = File::open(path).unwrap();
    let mut pbf_reader = OsmPbfReader::with_reader(osm_file);

    let mut g = Graph::new();
    let mut edges = Vec::new();
    let mut indices = Vec::new();

    for block in pbf_reader.primitive_blocks().map(|b| b.unwrap()) {
        for obj in blocks::iter(&block) {
            match obj {
                objects::OsmObj::Node(ref n) =>
                    g.add_node(Node{ osm_id: n.id, lat: n.lat, lon: n.lon, adj: HashMap::new()}),
                objects::OsmObj::Way(ref w)  => {
                    for i in 0..(w.nodes.len()-1) {
                        edges.push(Edge{ osm_id: w.id, length: 0, max_speed: 0, driving_time: 0 });
                        indices.push((w.nodes[i], w.nodes[i+1]));
                    }
                }
                objects::OsmObj::Relation(_) => ()
            }
        }
    }
    println!("Parsed and added {} nodes..", g.nodes.len());

    for (mut e, (n1, n2)) in edges.into_iter().zip(indices.into_iter()) {
        e.length = graph::haversine_length(&g.nodes[g.nodes_idx[&n1]], &g.nodes[g.nodes_idx[&n2]]) as u32;
        g.add_edge(n1, n2, e);
    }
    println!("Added {} edges..", g.edges.len());
    g.finalize();
    let graph = &g; // immutable reference to copy into the closures below

    println!("Calculating shortest paths for the first {} nodes..", NUM_NODES);
    let num_nodes = NUM_NODES / num_threads;
    let mut guards = Vec::with_capacity(num_threads);

    // Spawn NUM_THREADS amount of worker threads to calculate in parallel
    for id in 0..num_threads {
        guards.push(thread::scoped(move || {
            let first = id * num_nodes;
            let last = first + num_nodes;
            let mut dg = DijkstraGraph::from_graph(graph);
            println!("[Thread #{}] Starting calculation from {} to {}", id, first, last);
            for n in first..last {
                dg.dijkstra(n);
                if (n+1)%1000 == 0 {
                    println!("[Thread #{}] Finished node #{}", id, n+1);
                }
            }
        }));
    }

    // Join all threads before exiting
    for g in guards {
        g.join();
    }
    println!("Finished all calculations!")
}
