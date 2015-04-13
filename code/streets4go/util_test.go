package main

import (
    "container/heap"
    "testing"
)

func TestPriorityQueue(t *testing.T) {
    pq := PriorityQueue{}
    c := 1
    heap.Init(&pq)
    heap.Push(&pq, NodeState{5, c})
    c++
    heap.Push(&pq, NodeState{1, c})
    c++
    if l := pq.Len(); l != 2 {
        t.Error("Len != 2")
    }
    elem := heap.Pop(&pq).(NodeState)
    if elem.idx != 2 {
        t.Error("elem.idx != 2")
    }
    if elem.cost != 1 {
        t.Error("elem.cost != 1")
    }
}
