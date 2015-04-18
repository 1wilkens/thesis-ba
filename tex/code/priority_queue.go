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
