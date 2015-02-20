package example

import "fmt"

// Number of gorountines to start
const GOROUTINES = 4

func example() {
	// Create a channel to track completion
	c := make(chan int)

	for i := 0; i < GOROUTINES; i++ {
		// Start a goroutine
		go func(nr int) {
			fmt.Printf("Hello from routine %v", nr)
			// Signalize completion via channel
			c <- 1
		}(i)
	}

	for i := 0; i < GOROUTINES; i++ {
		// Wait for completion of all goroutines
		<-c
	}
}
