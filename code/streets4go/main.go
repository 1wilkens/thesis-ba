package main

import (
    "fmt"
    "io"
    "log"
    "os"
    "runtime"

    "github.com/qedus/osmpbf"
)

const (
    NODES = 10
    EDGES = 16

    MAX_WEIGTH = 14
)

func main() {
    fmt.Println("This is streets4go")

    osmPath := os.Args[1]
    //countOsm(osmPath)

    //testGraphStructure()

    benchmarkOsm(osmPath)

    printPerformanceStatistics()
}

func printPerformanceStatistics() {
    var m runtime.MemStats
    runtime.ReadMemStats(&m)
    fmt.Printf("total heap usage: %d allocs, %d frees, %d bytes allocated\n", m.Mallocs, m.Frees, m.TotalAlloc)
}

func countOsm(path string) {
    osmFile, err := os.Open(path)
    if err != nil {
        log.Fatal(err)
    }
    defer osmFile.Close()

    pbfReader := osmpbf.NewDecoder(osmFile)
    //runtime.GOMAXPROCS(runtime.NumCPU())
    fmt.Printf("Opening osm file: %s\n", path)
    err = pbfReader.Start(runtime.GOMAXPROCS(-1)) // use several goroutines for faster decoding
    if err != nil {
        log.Fatal(err)
    }

    var nodes, ways, relations uint64
    for {
        obj, err := pbfReader.Decode()

        if err == io.EOF {
            break
        } else if err != nil {
            log.Fatal(err)
        } else {
            switch obj := obj.(type) {
            case *osmpbf.Node:
                nodes++
                break
            case *osmpbf.Way:
                ways++
                break
            case *osmpbf.Relation:
                relations++
                break
            default:
                log.Fatalf("Found unknow type: %T\n", obj)
                break
            }
        }
    }
    fmt.Printf("Found %d nodes, %d ways and %d relations in %s\n", nodes, ways, relations, path)
}

func testGraphStructure() {
    g := NewGraph()
    for i := 1; i <= NODES; i++ {
        g.AddNode(&Node{osmID: int64(i * 5), adj: make(map[int]int)})
    }
    for i := 0; i < EDGES/2; i++ {
        n1, n2, n3 := i*5, (i+2)*5, (i+5)*5
        g.AddEdge(int64(n1), int64(n2), &Edge{osmID: int64(i), drivingTime: uint((i % MAX_WEIGTH) + 1)})
        g.AddEdge(int64(n1), int64(n3), &Edge{osmID: int64(i + EDGES), drivingTime: uint((i + 5%MAX_WEIGTH) + 1)})
    }

    dg := FromGraph(g)
    dg.g.Print()

    dg.Dijkstra(0)
    s := &dg.g.nodes[0]

    for i := 1; i < 10; i++ {
        p := &dg.g.nodes[i]
        if dg.dist[i] == INFINITY {
            fmt.Printf("Skipping %d since its unreachable!\n", p.osmID)
            continue
        }
        fmt.Printf("Started @ %d with distance: %d\n", p.osmID, dg.dist[i])
        for s != p {
            p = &dg.g.nodes[dg.parents[dg.g.nodeIdx[p.osmID]]]
            fmt.Printf("Discovered %d\n", p.osmID)
        }
        fmt.Println("Finished!")
    }
}

func benchmarkOsm(path string) {
    osmFile, err := os.Open(path)
    if err != nil {
        log.Fatal(err)
    }
    defer osmFile.Close()

    pbfReader := osmpbf.NewDecoder(osmFile)
    //runtime.GOMAXPROCS(runtime.NumCPU())
    fmt.Printf("Opening osm file: %s\n", path)
    err = pbfReader.Start(runtime.GOMAXPROCS(-1)) // use several goroutines for faster decoding
    if err != nil {
        log.Fatal(err)
    }

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

    fmt.Println("Calculating shortest paths for all nodes..")
    dg := FromGraph(g)
    for i := 0; i < 100000; i++ {
        dg.Dijkstra(i)
        if (i+1)%10000 == 0 {
            fmt.Printf("Finished node #%d\n", i+1)
        }
    }
}
