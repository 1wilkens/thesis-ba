/* in main() */
for block in pbf_reader.primitive_blocks().map(|b| b.unwrap()) {
    for obj in blocks::iter(&block) {
        match obj {
            objects::OsmObj::Node(_)        => nodes += 1,
            objects::OsmObj::Way(_)         => ways += 1,
            objects::OsmObj::Relation(_)    => rels += 1
        }
    }
}
/* remaining part of main() */
