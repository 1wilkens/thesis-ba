type Node struct {
	osmID    int64
	lon, lat float64

	adj map[int]int    // == adjecent edges/nodes
}

type Edge struct {
	osmID       int64
	length      int
	drivingTime uint   // == edge weight
	maxSpeed    uint8
}

type Graph struct {
	nodes []Node
	edges []Edge

	nodeIdx, edgeIdx map[int64]int
}

type DijkstraGraph struct {
	g *Graph
	pq PriorityQueue

	dist    []uint
	parents []int
}
