\mainpage

# wilderfield
The wilderfield library is a collection of header only libraries I've developed over time for various purposes.

## wilderfield::PriorityMap
The `PriorityMap` class is designed to manage a set of elements, each with a priority,  
allowing for efficient retrieval, updating, and tracking of elements based on their priority.  
  
It is designed to provide constant time access to the element with the highest priority,  
as well as constant time priority increment and decrement.  
  
Setting priority for a given element directly is O(n).  
  
The underlying implementation is a doubly linked list backed by a hashmap.  
  
The source code for this project is available on GitHub: [wilderfield/wilderfield](https://github.com/wilderfield/wilderfield)

## usage
Here is a basic example of how to use the `PriorityMap` class:
```cpp
#include "wilderfield/PriorityMap.hpp"
#include <iostream>

int main() {

  // Define a PriorityMap instance
  wilderfield::PriorityMap<int, int> pmap;

  // Increment priority of key 7
  ++pmap[7];

  // Retrieve and check the top element
  auto [key, value] = pmap.Top();
  std::cout << "Top element key: " << key << " with priority: " << value << std::endl;

  return 0;
}
```

## example application

Below is an example application illustrating topological sort with Khan's algorithm and a priority map:

```cpp
#include "wilderfield/PriorityMap.hpp"
#include <vector>
#include <cassert>

std::vector<int> TopSort(std::vector<std::vector<int>>& graph) {
 
   std::vector<int> result;
 
   // Construct priority map with lower values as highest priority
   wilderfield::PriorityMap<int, int, std::less<int>> pmap;
 
   // Initialize pmap to have all nodes with indegree 0
   for (int u = 0; u < graph.size(); u++) {
     pmap[u] = 0;
   }
 
   // Calculate indegrees for each node
   for (int u = 0; u < graph.size(); u++) {
     for (auto v : graph[u]) {
         ++pmap[v];
     }
   }
 
   // Iterate over nodes by smallest indegree
   while (!pmap.Empty()) {
     auto [u, min_val] = pmap.Top(); pmap.Pop();
     assert(min_val == 0); // If not the graph has a cycle
     result.push_back(u);
     for (auto v : graph[u]) { // Decrease indegrees after removing node u
       --pmap[v];
     }
   }
 
   return result;
}
```

# build, test, benchmark, install
Typical CMake build, tune commands to your platform/generator
```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake —-build build
build/tests/priority_map_test -s
build/benchmark/run_benchmarking
cmake --install build
```

# documentation 
For more information and advanced usage, refer to the respective class and method documentation.  
  
The documentation for this project is available on GitHub: [wilderfield/wilderfield documentation/(https://wilderfield.github.io/wilderfield)
