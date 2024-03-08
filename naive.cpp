#include <iostream>
#include <vector>
#include <climits>

using namespace std;

using Graph = vector<vector<int>>;

// Function to check if a set of vertices is a dominating set
bool isDominatingSet(Graph &graph, vector<int> &set) {
    int n = graph.size();
    vector<bool> dominated(n, false);

    // Mark all vertices in the set as dominated
    for (int vertex: set) {
        dominated[vertex] = true;
        // Mark all neighbors as dominated
        for (int neighbor: graph[vertex]) {
            dominated[neighbor] = true;
        }
    }

    // Check if all vertices are dominated
    for (bool vertex: dominated) {
        if (!vertex) return false;
    }
    return true;
}

// Utility function to find the smallest dominating set by checking all subsets
int findSmallestDominatingSet(Graph &graph) {
    int n = graph.size();
    int smallestSize = INT_MAX;

    // Iterate through all possible subsets of vertices
    for (int i = 1; i < (1 << n); ++i) {
        vector<int> currentSet;
        for (int j = 0; j < n; ++j) {
            if (i & (1 << j)) {
                currentSet.push_back(j);
            }
        }

        // Check if the current set is a dominating set
        if (isDominatingSet(graph, currentSet)) {
            smallestSize = min(smallestSize, (int) currentSet.size());
        }
    }

    return smallestSize;
}

int main() {
    // Example graph represented as an adjacency list
    // Graph vertices are 0-indexed
    Graph graph = {
            {1, 2},    // Neighbors of vertex 0
            {0, 3, 4}, // Neighbors of vertex 1
            {0, 4},    // Neighbors of vertex 2
            {1, 4},    // Neighbors of vertex 3
            {1, 2, 3}  // Neighbors of vertex 4
    };

    cout << "The size of the smallest dominating set is: "
         << findSmallestDominatingSet(graph) << endl;

    return 0;
}
