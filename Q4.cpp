#include<bits/stdc++.h>
using namespace std;

// Using long long for Dijkstra distances to prevent overflow during addition
const long long INF = numeric_limits<long long>::max();

class Graph {
    int V;
    bool directed;
    // adj is for unweighted, lexicographical traversals
    vector<set<int>> adj; 
    // weightedAdj is for Dijkstra
    vector<vector<pair<int, int>>> weightedAdj;

    // --- DFS Util (for cycle in directed graph) ---
    bool hasCycleUtil_Directed(int u, vector<bool> &vis, vector<bool> &recStack) {
        vis[u] = true;
        recStack[u] = true;
        for (int v : adj[u]) {
            if (!vis[v]) {
                if (hasCycleUtil_Directed(v, vis, recStack)) return true;
            } else if (recStack[v]) {
                return true;
            }
        }
        recStack[u] = false;
        return false;
    }

    // --- DFS Util (for cycle in undirected graph) ---
    bool hasCycleUtil_Undirected(int u, int parent, vector<bool> &vis) {
        vis[u] = true;
        for (int v : adj[u]) {
            if (v == parent) continue; // Skip the edge back to the parent
            if (vis[v]) return true;  // Found a back-edge to an ancestor
            if (hasCycleUtil_Undirected(v, u, vis)) return true;
        }
        return false;
    }

    // --- DFS Util (for Kosaraju's 1st pass) ---
    void dfs1_fillOrder(int u, vector<bool> &vis, stack<int> &st) {
        vis[u] = true;
        for (int v : adj[u]) {
            if (!vis[v]) {
                dfs1_fillOrder(v, vis, st);
            }
        }
        st.push(u);
    }

    // --- DFS Util (for Kosaraju's 2nd pass) ---
    void dfs2_collect(int u, vector<bool> &vis, vector<int> &comp, const vector<set<int>> &radj) {
        vis[u] = true;
        comp.push_back(u);
        for (int v : radj[u]) {
            if (!vis[v]) {
                dfs2_collect(v, vis, comp, radj);
            }
        }
    }

public:
    Graph(int n, bool dir) : V(n), directed(dir), adj(n), weightedAdj(n) {}

    // Type 9: Add Vertex
    int addVertex() {
        int newID = V; // New vertex ID is the current size
        V++;
        adj.resize(V);
        weightedAdj.resize(V);
        return newID; // Return the new ID
    }

    // Type 8: Add Edge
    void addEdge(int u, int v, int w) {
        if (u >= V || v >= V) return;
        
        // Add to unweighted list for BFS/DFS/etc.
        adj[u].insert(v);
        // Add to weighted list for Dijkstra
        weightedAdj[u].push_back({v, w});

        if (!directed) {
            adj[v].insert(u);
            weightedAdj[v].push_back({u, w});
        }
    }

    // Type 0: Lexicographically smallest BFS
    vector<int> bfs(int start) {
        vector<int> order;
        if (start >= V) return order;
        vector<bool> vis(V, false);
        queue<int> q;
        q.push(start);
        vis[start] = true;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            order.push_back(u);
            // 'adj' is a set, so neighbors are already sorted
            for (int v : adj[u]) {
                if (!vis[v]) {
                    vis[v] = true;
                    q.push(v);
                }
            }
        }
        return order;
    }

    // --- DFS Util (for main DFS traversal) ---
    void dfsUtil(int u, vector<bool> &vis, vector<int> &res) {
        vis[u] = true;
        res.push_back(u);
        // 'adj' is a set, so neighbors are already sorted
        for (int v : adj[u]) {
            if (!vis[v]) {
                dfsUtil(v, vis, res);
            }
        }
    }

    // Type 1: Lexicographically smallest DFS
    vector<int> dfs(int start) {
        vector<int> res;
        if (start >= V) return res;
        vector<bool> vis(V, false);
        dfsUtil(start, vis, res);
        return res;
    }

    // Type 2: Cycle Detection
    bool hasCycle() {
        vector<bool> vis(V, false);
        if (directed) {
            vector<bool> recStack(V, false);
            for (int i = 0; i < V; i++) {
                if (!vis[i]) {
                    if (hasCycleUtil_Directed(i, vis, recStack)) return true;
                }
            }
        } else {
            for (int i = 0; i < V; i++) {
                if (!vis[i]) {
                    if (hasCycleUtil_Undirected(i, -1, vis)) return true;
                }
            }
        }
        return false;
    }

    // Type 3: Bipartiteness Check (for undirected)
    bool isBipartite() {
        if (directed) return false; // Per PDF, only for undirected
        vector<int> color(V, -1);
        for (int i = 0; i < V; i++) {
            if (color[i] == -1) {
                queue<int> q;
                q.push(i);
                color[i] = 0;
                while (!q.empty()) {
                    int u = q.front(); q.pop();
                    for (int v : adj[u]) {
                        if (color[v] == -1) {
                            color[v] = 1 - color[u];
                            q.push(v);
                        } else if (color[v] == color[u]) {
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }

    // Type 4 (Undirected): Connected Components
    int connectedComponents() {
        int count = 0;
        vector<bool> vis(V, false);
        for (int i = 0; i < V; i++) {
            if (!vis[i]) {
                count++;
                queue<int> q;
                q.push(i);
                vis[i] = true;
                while (!q.empty()) {
                    int u = q.front(); q.pop();
                    for (int v : adj[u]) {
                        if (!vis[v]) {
                            vis[v] = true;
                            q.push(v);
                        }
                    }
                }
            }
        }
        return count;
    }

    // Type 4 (Directed) / Type 6: Kosaraju's SCC
    vector<vector<int>> stronglyConnectedComponents() {
        stack<int> st;
        vector<bool> vis(V, false);
        for (int i = 0; i < V; i++) {
            if (!vis[i]) {
                dfs1_fillOrder(i, vis, st);
            }
        }

        // Create reversed graph (transpose)
        vector<set<int>> radj(V);
        for (int u = 0; u < V; u++) {
            for (int v : adj[u]) {
                radj[v].insert(u);
            }
        }

        fill(vis.begin(), vis.end(), false);
        vector<vector<int>> scc;
        while (!st.empty()) {
            int u = st.top(); 
            st.pop();
            if (!vis[u]) {
                vector<int> comp;
                dfs2_collect(u, vis, comp, radj);
                sort(comp.begin(), comp.end()); // Sort component lexicographically
                scc.push_back(comp);
            }
        }
        return scc;
    }

    // Type 5: Dijkstra's Algorithm
    vector<long long> dijkstra(int src) {
        vector<long long> dist(V, INF);
        dist[src] = 0;
        // Min-priority queue: {distance, vertex}
        priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<pair<long long, int>>> pq;
        pq.push({0, src});

        while (!pq.empty()) {
            long long d = pq.top().first;
            int u = pq.top().second;
            pq.pop();

            if (d > dist[u]) continue; // Stale entry

            for (auto &edge : weightedAdj[u]) {
                int v = edge.first;
                int w = edge.second;
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    pq.push({dist[v], v});
                }
            }
        }
        return dist;
    }

    // Type 7: Topological Sort
    vector<int> topoSort() {
        vector<int> indeg(V, 0);
        for (int u = 0; u < V; u++) {
            for (int v : adj[u]) {
                indeg[v]++;
            }
        }

        // Use min-priority queue for lexicographically smallest
        priority_queue<int, vector<int>, greater<int>> q;
        for (int i = 0; i < V; i++) {
            if (indeg[i] == 0) {
                q.push(i);
            }
        }

        vector<int> order;
        while (!q.empty()) {
            int u = q.top(); 
            q.pop();
            order.push_back(u);
            for (int v : adj[u]) {
                indeg[v]--;
                if (indeg[v] == 0) {
                    q.push(v);
                }
            }
        }
        return order;
    }

    // --- Getter for 'directed' property ---
    bool isDirected() const {
        return directed;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int V, E;
    bool isDirected;
    cin >> isDirected >> V >> E;
    Graph g(V, isDirected);

    for (int i = 0; i < E; i++) {
        int u, v, w;
        cin >> u >> v >> w; // Read u, v, and weight w
        g.addEdge(u, v, w);
    }

    int Q;
    cin >> Q; // Number of queries
    for (int i = 0; i < Q; i++) {
        int queryType;
        cin >> queryType;

        switch (queryType) {
            case 0: { // BFS
                int s; cin >> s;
                auto res = g.bfs(s);
                for (int j = 0; j < res.size(); j++) {
                    cout << res[j] << (j == res.size() - 1 ? "" : " ");
                }
                cout << "\n";
                break;
            }
            case 1: { // DFS
                int s; cin >> s;
                auto res = g.dfs(s);
                for (int j = 0; j < res.size(); j++) {
                    cout << res[j] << (j == res.size() - 1 ? "" : " ");
                }
                cout << "\n";
                break;
            }
            case 2: { // Cycle Detection
                cout << (g.hasCycle() ? "1" : "0") << "\n";
                break;
            }
            case 3: { // Bipartiteness Check
                if (g.isDirected()) {
                     // As per PDF, skip if directed
                } else {
                    cout << (g.isBipartite() ? "1" : "0") << "\n";
                }
                break;
            }
            case 4: { // Connected Components
                if (g.isDirected()) {
                    auto scc = g.stronglyConnectedComponents();
                    cout << scc.size() << "\n";
                } else {
                    cout << g.connectedComponents() << "\n";
                }
                break;
            }
            case 5: { // Dijkstra's
                int s, t; cin >> s >> t;
                auto dists = g.dijkstra(s);
                if (dists[t] == INF) {
                    cout << -1 << "\n"; // Or some indicator of no path
                } else {
                    cout << dists[t] << "\n";
                }
                break;
            }
            case 6: { // Kosaraju's SCCs
                if (g.isDirected()) {
                    auto scc = g.stronglyConnectedComponents();
                    for (auto &comp : scc) {
                        for (int j = 0; j < comp.size(); j++) {
                            cout << comp[j] << (j == comp.size() - 1 ? "" : " ");
                        }
                        cout << "\n";
                    }
                }
                break;
            }
            case 7: { // Topological Sort
                if (g.isDirected()) {
                    auto res = g.topoSort();
                    for (int j = 0; j < res.size(); j++) {
                        cout << res[j] << (j == res.size() - 1 ? "" : " ");
                    }
                    cout << "\n";
                }
                break;
            }
            case 8: { // Add Edge
                int u, v, w; cin >> u >> v >> w;
                g.addEdge(u, v, w);
                break;
            }
            case 9: { // Add Vertex
                int newID = g.addVertex();
                cout << newID << "\n";
                break;
            }
        }
    }

    return 0;
}