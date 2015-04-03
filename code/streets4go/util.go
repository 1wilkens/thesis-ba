package main

import "math"

// taken and modified from http://rosettacode.org/wiki/Priority_queue#Go
type NodeState struct {
	cost uint
	idx  int
}

type PriorityQueue []NodeState

func (self PriorityQueue) Len() int {
	return len(self)
}
func (self PriorityQueue) Less(i, j int) bool {
	return self[i].cost < self[j].cost
}

func (self PriorityQueue) Swap(i, j int) {
	self[i], self[j] = self[j], self[i]
}

func (self *PriorityQueue) Push(x interface{}) {
	*self = append(*self, x.(NodeState))
}

func (self *PriorityQueue) Pop() (popped interface{}) {
	popped = (*self)[len(*self)-1]
	*self = (*self)[:len(*self)-1]
	return
}

const conversionFactor = math.Pi / 180

func HaversineLength(n1, n2 *Node) float64 {
	lat1, lon1, lat2, lon2 := Rad(n1.lat), Rad(n1.lon), Rad(n2.lat), Rad(n2.lon)
	dlat, dlon := lat2-lat1, lon2-lon1
	a := math.Pow(math.Sin(dlat/2), 2) + math.Cos(lat1)*math.Cos(lat2)*math.Pow(math.Sin(dlon/2), 2)
	c := 2 * math.Asin(math.Sqrt(a))
	return 6367000 * c // distance in m
}

func Rad(deg float64) float64 {
	return deg * conversionFactor
}
