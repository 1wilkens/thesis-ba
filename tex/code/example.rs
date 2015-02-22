// Immutability per default, Option type built-in -> no null
fn example(val: &i32, mutable: &mut i32) -> Option<String> {
    // Pattern matching
    match *val {
        /* Ranges types (x ... y notation),
         * Powerful macro system (called via 'macro!()') */
        v @ 1 ... 5 => Some(format!("In [1, 5]: {}", v)),
        // Conditional matching
        v if v < 10 => Some(format!("In [6,10): {}", v)),
        // Constant matching
        10          => Some("Exactly 10".to_string()),
        /* Exhaustiveness checks at compile time,
         * '_' matches everything */
        _           => None
    }
    // statements are expressions -> no need for 'return'
}
