package main

import (
	"fmt"
	"io"
	"log"
	"os"
	"runtime"
	"github.com/qedus/osmpbf" // <- add the import
)

func main() {
    decoder := osmpbf.NewDecoder(someFile)  // <- use some type or function from the package
}
