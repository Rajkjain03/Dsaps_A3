// Graph Explorer
#include <bits/stdc++.h>

using namespace std;

// BFS (Type 0): Perform Breadth-First Search from a given source vertex and
// print the lexicographically smallest BFS traversal order.

// DFS (Type 1): Perform Depth-First Search from a given source vertex and print
// the lexicographically smallest DFS traversal order.

// • Cycle Detection (Type 2): Check whether the graph contains a cycle. Works
// for both directed and undirected graphs.

// • Bipartiteness Check (Type 3): Determine if the graph is bipartite. Applicable
// only for undirected graphs.

// • Connected Components (Type 4): Count the number of connected components
// in the graph. For directed graphs, this refers to the number of strongly connected
// components.

// Dijkstra’s Algorithm (Type 5): Compute the shortest path from a source vertex
// to a target vertex in a weighted graph.

// Kosaraju’s Algorithm (Type 6): For directed graphs, print all strongly connected
// components (SCCs). Each SCC should be printed as a space-separated list
// on a new line, in lexicographical order.

// Topological Sort (Type 7): For directed acyclic graphs (DAGs), print the
//  lexicographically smallest topological order.

// Add Edge (Type 8): Dynamically add an edge (u, v, w) to the graph. Default
// weight is 1 if unweighted. The update should take effect immediately.

// Add Vertex (Type 9): Add a new vertex to the graph. The new vertex ID will
//  be the next unused integer (starting from V ). After adding the vertex, print its
//  assigned ID.

int main()
{
    ios::sync_with_stdio(0);
    cin.tie(NULL);

    int isDirected, V, E;
    cin >> isDirected >> V >> E;

    for (int i = 0; i < E; i++)
    {
        int u, v, w;
        cin >> u >> v >> w;
    }

    int Q;
    cin >> Q;

    while (Q--)
    {
        int type;
        cin >> type;

        switch (type)
        {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;

        case 8:
            break;
        case 9:
            break;
        default:
            cout << "Enter correct query type" << "\n";
        }
        

    return 0;
}