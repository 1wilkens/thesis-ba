package main

import (
	"container/heap"
	"fmt"
)

const (
	INFINITY = ^uint(0) // uint.MaxValue
)

type Node struct {
	osmID int64

	lon, lat float64

	adj map[int]int
}

type Edge struct {
	osmID int64

	length      int
	drivingTime uint
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

func NewGraph() *Graph {
	g := new(Graph)

	g.nodes = *new([]Node)
	g.edges = *new([]Edge)

	g.nodeIdx = make(map[int64]int)
	g.edgeIdx = make(map[int64]int)

	return g
}

func (g *Graph) AddNode(n *Node) {
	g.nodeIdx[n.osmID] = len(g.nodes)
	g.nodes = append(g.nodes, *n)
}

func (g *Graph) AddEdge(n1, n2 int64, e *Edge) {
	if _, ok := g.nodeIdx[n1]; !ok {
		return
	}
	if _, ok := g.nodeIdx[n2]; !ok {
		return
	}

	// add edge
	idx := len(g.edges)
	g.edges = append(g.edges, *e)
	g.edgeIdx[e.osmID] = idx

	// link up adjecents
	n1_idx, n2_idx := g.nodeIdx[n1], g.nodeIdx[n2]
	g.nodes[n1_idx].adj[n2_idx] = idx
	g.nodes[n2_idx].adj[n1_idx] = idx
}

func (g *Graph) Print() {
	fmt.Printf("Nodes (%d):\n", len(g.nodes))
	for n := range g.nodes {
		node := g.nodes[n]
		fmt.Printf("(osm: %d, id: %d)\n", node.osmID, g.nodeIdx[node.osmID])
	}
	fmt.Printf("\nEdges (%d):\n", len(g.edges))
	for n := range g.nodes {
		node := g.nodes[n]
		for adj, e := range node.adj {
			fmt.Printf("N%d -> N%d [label=%d];\n", node.osmID, g.nodes[adj].osmID, g.edges[e].drivingTime)
		}
	}
	fmt.Println("")
}

func FromGraph(g *Graph) (dg *DijkstraGraph) {
	dg = new(DijkstraGraph)
	dg.g = g

	dg.dist = make([]uint, len(g.nodes))
	dg.parents = make([]int, len(g.nodes))
	return
}

func (dg *DijkstraGraph) Dijkstra(start int) {
	for i := range dg.g.nodes {
		dg.dist[i] = INFINITY
		dg.parents[i] = -1
	}
	dg.dist[start] = 0

	dg.pq = PriorityQueue{{0, start}}
	heap.Init(&dg.pq) // required to build up intial heap

	for dg.pq.Len() > 0 {
		cur := heap.Pop(&dg.pq).(NodeState)

		if cur.cost <= dg.dist[cur.idx] {
			node := dg.g.nodes[cur.idx]

			// relax all adjecent edges
			for node, edge := range node.adj {
				w_cur := dg.dist[node]
				w_new := dg.dist[cur.idx] + dg.g.edges[edge].drivingTime

				if w_new < w_cur {
					dg.dist[node] = w_new
					dg.parents[node] = cur.idx
					heap.Push(&dg.pq, NodeState{w_new, node})
				}
			}
		}
	}
}
