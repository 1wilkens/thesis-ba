package main

import (
    "fmt"
    "io"
    "log"
    "strconv"
    "sync"
    "os"
    "runtime"

    "github.com/qedus/osmpbf"
)

const (
    NUM_NODES = 100000
    NUM_THREADS = 4
)

func main() {
    path := os.Args[1]
    num_threads := NUM_THREADS;
    if len(os.Args) > 2 {
        num_threads, _ = strconv.Atoi(os.Args[2])
    }
    fmt.Println("This is streets4go")
    fmt.Printf("Starting with NUM_THREADS=%d, NUM_NODES=%d\n", num_threads, NUM_NODES);

    osmFile, _ := os.Open(path)
    defer osmFile.Close()

    pbfReader := osmpbf.NewDecoder(osmFile)
    runtime.GOMAXPROCS(runtime.NumCPU())
    fmt.Printf("Opening osm file: %s\n", path)
    _ = pbfReader.Start(runtime.GOMAXPROCS(-1)) // use several goroutines for faster decoding

    g := NewGraph()
    // edges cannot be directly added since the nodes might not have been discovered yet
    edges := *new([]Edge)
    n1, n2 := *new([]int64), *new([]int64)

    for {
        obj, err := pbfReader.Decode()

        if err == io.EOF {
            break
        } else if err != nil {
            log.Fatal(err)
        } else {
            switch obj := obj.(type) {
            case *osmpbf.Node:
                g.AddNode(&Node{osmID: obj.ID, lon: obj.Lon, lat: obj.Lat, adj: make(map[int]int)})
                break
            case *osmpbf.Way:
                for i := 0; i < len(obj.NodeIDs) - 1; i++ {
                    edges = append(edges, Edge{osmID: obj.ID})
                    n1 = append(n1, obj.NodeIDs[i])
                    n2 = append(n2, obj.NodeIDs[i+1])
                }
                break
            case *osmpbf.Relation:
                break
            default:
                log.Fatalf("Found unknow type: %T\n", obj)
                break
            }
        }
    }
    fmt.Printf("Parsed and added %d nodes..\n", len(g.nodes))

    for i := range edges {
        e := edges[i]
        e.length = int(HaversineLength(&g.nodes[g.nodeIdx[n1[i]]], &g.nodes[g.nodeIdx[n2[i]]]))
        g.AddEdge(n1[i], n2[i], &e)
    }
    fmt.Printf("Added %d edges..\n", len(g.edges))

    fmt.Printf("Calculating shortest paths for the first %d nodes..\n", NUM_NODES)
    nodes_per_thread := NUM_NODES / num_threads;

    // To wait goroutines later on
    var wg sync.WaitGroup
    runtime.GOMAXPROCS(num_threads)

    for i := 0; i < num_threads; i++ {
        wg.Add(1)   // Increase WaitGroup counter
        go func(id int) {
            defer wg.Done() // Decrease counter when the goroutine exits

            dg := FromGraph(g)
            first := id * nodes_per_thread
            last := first + nodes_per_thread
            fmt.Printf("[Goroutine #%d] Starting calculation from %d to %d\n", id, first, last)
            for n := first; n < last; n++ {
                dg.Dijkstra(n)
                if (n+1)%1000 == 0 {
                    fmt.Printf("[Goroutine #%d] Finished node #%d\n", id, n+1)
                }
            }
        }(i)
    }

    wg.Wait()
    fmt.Println("Finished all calculations!")
}
