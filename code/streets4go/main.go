package main

import (
	"fmt"
	"io"
	"log"
	"os"
	"runtime"

	"github.com/qedus/osmpbf"
)

func main() {
	fmt.Println("This is streets4go")

	osmPath := os.Args[1]
	osmFile, err := os.Open(osmPath)
	if err != nil {
		log.Fatal(err)
	}
	defer osmFile.Close()

	pbfReader := osmpbf.NewDecoder(osmFile)
	//runtime.GOMAXPROCS(runtime.NumCPU())
	fmt.Printf("Opening osm file: %s\n", osmPath)
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
	fmt.Printf("Found %d nodes, %d ways and %d relations in %s\n", nodes, ways, relations, osmPath)
}
