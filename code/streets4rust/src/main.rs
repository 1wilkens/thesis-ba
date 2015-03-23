extern crate osmpbfreader;

use std::env;
use std::fs::File;
use std::path::Path;

use osmpbfreader::{OsmPbfReader, blocks, objects};

#[allow(dead_code)]
mod graph;

fn main() {
    println!("This is streets4rust\n");

    //let args: Vec<String> = env::args().collect();
    //count_osm(Path::new(&args[1]));

}

fn count_osm(osm_path: Path) {
    println!("Opening osm file: {}", osm_path.display());
    let osm_file = File::open(&Path::new(&args[1])).unwrap();
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
