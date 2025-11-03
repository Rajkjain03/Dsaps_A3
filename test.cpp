// Q4_GraphExplorer.cpp
// Compile: g++ -std=c++17 Q4_GraphExplorer.cpp -O2 -o graph
// Usage: provide input as described in assignment PDF.

#include <bits/stdc++.h>
using namespace std;
using ll = long long;
const ll INFLL = (1LL<<60);

struct Edge {
    int to;
    int w;
    Edge(int _to=0, int _w=1): to(_to), w(_w) {}
};

struct GraphExplorer {
    bool isDirected;
    int V; // current number of vertices
    vector<vector<Edge>> adj;      // adjacency list (for main graph)
    vector<vector<Edge>> adj_rev;  // reverse adjacency (for directed Kosaraju)
    GraphExplorer(bool _isDirected=false, int _V=0) {
        isDirected = _isDirected;
        V = _V;
        adj.assign(V, {});
        adj_rev.assign(V, {});
    }

    void ensure_vertex_exists(int u) {
        if (u < 0) return;
        while ((int)adj.size() <= u) {
            adj.emplace_back();
            adj_rev.emplace_back();
            ++V;
        }
    }

    // add edge (u,v,w), no duplicate check (assignment says multi-edges and self-loops are not allowed,
    // but we'll accept input; duplicates don't break functionality)
    void add_edge(int u, int v, int w=1) {
        ensure_vertex_exists(max(u,v));
        adj[u].push_back(Edge(v,w));
        if (isDirected) {
            adj_rev[v].push_back(Edge(u,w)); // reverse graph for Kosaraju (weights don't matter)
        } else {
            adj[v].push_back(Edge(u,w));
            // for undirected, adj_rev not used but keep symmetric
        }
    }

    // add vertex: returns new vertex id
    int add_vertex() {
        int id = V;
        adj.emplace_back();
        adj_rev.emplace_back();
        ++V;
        return id;
    }

    // BFS lexicographically smallest from s: returns vector of visited in order
    vector<int> bfs_lex(int s) {
        vector<int> order;
        if (s < 0 || s >= V) return order;
        vector<char> vis(V, 0);
        queue<int> q;
        vis[s] = 1;
        q.push(s);
        while (!q.empty()) {
            int u = q.front(); q.pop();
            order.push_back(u);
            // collect neighbors and sort to ensure lexicographically smallest exploration
            vector<int> neigh;
            neigh.reserve(adj[u].size());
            for (auto &e : adj[u]) neigh.push_back(e.to);
            sort(neigh.begin(), neigh.end());
            for (int v : neigh) {
                if (!vis[v]) {
                    vis[v] = 1;
                    q.push(v);
                }
            }
        }
        return order;
    }

    // DFS lexicographically smallest from s: returns traversal order (lexicographically smallest)
    vector<int> dfs_lex(int s) {
        vector<int> order;
        if (s < 0 || s >= V) return order;
        vector<char> vis(V, 0);
        function<void(int)> dfs = [&](int u) {
            vis[u] = 1;
            order.push_back(u);
            vector<int> neigh;
            neigh.reserve(adj[u].size());
            for (auto &e : adj[u]) neigh.push_back(e.to);
            sort(neigh.begin(), neigh.end());
            for (int v : neigh) if (!vis[v]) dfs(v);
        };
        dfs(s);
        return order;
    }

    // Cycle detection
    bool cycle_directed() {
        vector<int> color(V, 0); // 0 unvisited, 1 visiting, 2 done
        function<bool(int)> dfs = [&](int u) -> bool {
            color[u] = 1;
            // neighbors in increasing order not necessary for correctness
            for (auto &e : adj[u]) {
                int v = e.to;
                if (color[v] == 0) {
                    if (dfs(v)) return true;
                } else if (color[v] == 1) {
                    return true;
                }
            }
            color[u] = 2;
            return false;
        };
        for (int i = 0; i < V; ++i) {
            if (color[i] == 0) {
                if (dfs(i)) return true;
            }
        }
        return false;
    }

    bool cycle_undirected() {
        vector<char> vis(V, 0);
        function<bool(int,int)> dfs = [&](int u, int p)->bool {
            vis[u] = 1;
            for (auto &e : adj[u]) {
                int v = e.to;
                if (!vis[v]) {
                    if (dfs(v, u)) return true;
                } else if (v != p) {
                    return true; // back edge to non-parent
                }
            }
            return false;
        };
        for (int i = 0; i < V; ++i) {
            if (!vis[i]) {
                if (dfs(i, -1)) return true;
            }
        }
        return false;
    }

    // bipartite check for undirected
    bool bipartite_undirected() {
        if (isDirected) return false; // unsupported
        vector<int> color(V, -1);
        for (int i = 0; i < V; ++i) {
            if (color[i] != -1) continue;
            // BFS from i
            queue<int> q;
            color[i] = 0;
            q.push(i);
            while (!q.empty()) {
                int u = q.front(); q.pop();
                for (auto &e : adj[u]) {
                    int v = e.to;
                    if (color[v] == -1) {
                        color[v] = 1 - color[u];
                        q.push(v);
                    } else if (color[v] == color[u]) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    // Connected components:
    // undirected -> number of connected components
    // directed -> number of SCCs (Kosaraju)
    int count_connected_components() {
        if (!isDirected) {
            vector<char> vis(V, 0);
            int cnt = 0;
            for (int i = 0; i < V; ++i) {
                if (!vis[i]) {
                    ++cnt;
                    // BFS/DFS
                    stack<int> st;
                    st.push(i); vis[i] = 1;
                    while (!st.empty()) {
                        int u = st.top(); st.pop();
                        for (auto &e : adj[u]) {
                            int v = e.to;
                            if (!vis[v]) {
                                vis[v] = 1;
                                st.push(v);
                            }
                        }
                    }
                }
            }
            return cnt;
        } else {
            // number of SCCs using Kosaraju
            vector<char> vis(V, 0);
            vector<int> order;
            order.reserve(V);
            function<void(int)> dfs1 = [&](int u) {
                vis[u] = 1;
                // traverse neighbors in ascending order for determinism, but not required for count
                vector<int> neigh;
                neigh.reserve(adj[u].size());
                for (auto &e : adj[u]) neigh.push_back(e.to);
                sort(neigh.begin(), neigh.end());
                for (int v : neigh) if (!vis[v]) dfs1(v);
                order.push_back(u);
            };
            for (int i = 0; i < V; ++i) if (!vis[i]) dfs1(i);
            fill(vis.begin(), vis.end(), 0);
            int sccCount = 0;
            function<void(int)> dfs2 = [&](int u) {
                vis[u] = 1;
                for (auto &e : adj_rev[u]) {
                    int v = e.to;
                    if (!vis[v]) dfs2(v);
                }
            };
            for (int i = (int)order.size()-1; i >= 0; --i) {
                int v = order[i];
                if (!vis[v]) {
                    ++sccCount;
                    dfs2(v);
                }
            }
            return sccCount;
        }
    }

    // Kosaraju to return SCCs (vectors of ints), if directed. If undirected, behavior: each connected component as a SCC.
    vector<vector<int>> kosaraju_sccs() {
        vector<vector<int>> result;
        if (!isDirected) {
            // for undirected graph, use connected components
            vector<char> vis(V,0);
            for (int i = 0; i < V; ++i) {
                if (!vis[i]) {
                    vector<int> comp;
                    stack<int> st; st.push(i); vis[i] = 1;
                    while (!st.empty()) {
                        int u = st.top(); st.pop();
                        comp.push_back(u);
                        for (auto &e : adj[u]) {
                            if (!vis[e.to]) { vis[e.to] = 1; st.push(e.to); }
                        }
                    }
                    sort(comp.begin(), comp.end());
                    result.push_back(comp);
                }
            }
            sort(result.begin(), result.end(), [](const vector<int>&a, const vector<int>&b){
                return a.front() < b.front();
            });
            return result;
        }
        // directed: Kosaraju
        vector<char> vis(V, 0);
        vector<int> order;
        order.reserve(V);
        function<void(int)> dfs1 = [&](int u) {
            vis[u] = 1;
            vector<int> neigh;
            neigh.reserve(adj[u].size());
            for (auto &e : adj[u]) neigh.push_back(e.to);
            sort(neigh.begin(), neigh.end());
            for (int v : neigh) if (!vis[v]) dfs1(v);
            order.push_back(u);
        };
        for (int i = 0; i < V; ++i) if (!vis[i]) dfs1(i);
        fill(vis.begin(), vis.end(), 0);
        function<void(int, vector<int>&)> dfs2 = [&](int u, vector<int> &comp) {
            vis[u] = 1;
            comp.push_back(u);
            for (auto &e : adj_rev[u]) {
                int v = e.to;
                if (!vis[v]) dfs2(v, comp);
            }
        };
        for (int i = (int)order.size() - 1; i >= 0; --i) {
            int v = order[i];
            if (!vis[v]) {
                vector<int> comp;
                dfs2(v, comp);
                sort(comp.begin(), comp.end());
                result.push_back(comp);
            }
        }
        sort(result.begin(), result.end(), [](const vector<int>&a, const vector<int>&b){
            return a.front() < b.front();
        });
        return result;
    }

    // Topological sort (lexicographically smallest) — only for directed graphs and DAGs
    // Returns (empty vector) if not a DAG or not directed
    vector<int> topological_sort_lex() {
        if (!isDirected) return {};
        vector<int> indeg(V,0);
        for (int u = 0; u < V; ++u)
            for (auto &e : adj[u]) indeg[e.to]++;

        // min-heap to pick smallest available node
        priority_queue<int, vector<int>, greater<int>> pq;
        for (int i = 0; i < V; ++i) if (indeg[i] == 0) pq.push(i);

        vector<int> topo;
        while (!pq.empty()) {
            int u = pq.top(); pq.pop();
            topo.push_back(u);
            // neighbors must be processed in increasing order to ensure determinism (though not required here)
            vector<int> neigh;
            for (auto &e : adj[u]) neigh.push_back(e.to);
            sort(neigh.begin(), neigh.end());
            for (int v : neigh) {
                indeg[v]--;
                if (indeg[v] == 0) pq.push(v);
            }
        }
        if ((int)topo.size() != V) return {}; // not a DAG
        return topo;
    }

    // Dijkstra shortest distance from s to t (returns -1 if unreachable)
    long long dijkstra(int s, int t) {
        if (s < 0 || s >= V || t < 0 || t >= V) return -1;
        vector<ll> dist(V, INFLL);
        using pli = pair<ll,int>;
        priority_queue<pli, vector<pli>, greater<pli>> pq;
        dist[s] = 0; pq.push({0,s});
        while (!pq.empty()) {
            auto [d,u] = pq.top(); pq.pop();
            if (d != dist[u]) continue;
            if (u == t) return d;
            for (auto &e : adj[u]) {
                int v = e.to; ll nd = d + e.w;
                if (nd < dist[v]) {
                    dist[v] = nd;
                    pq.push({nd, v});
                }
            }
        }
        if (dist[t] == INFLL) return -1;
        return dist[t];
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int isDirectedFlag;
    int V, E;
    if (!(cin >> isDirectedFlag >> V >> E)) return 0;
    bool isDirected = (isDirectedFlag == 1);
    GraphExplorer ge(isDirected, V);

    // Read E edges; each line: u v [w] (weight optional)
    // Use getline+stringstream to allow optional weight
    string rest;
    getline(cin, rest); // consume endline
    for (int i = 0; i < E; ++i) {
        string line;
        getline(cin, line);
        if (line.size() == 0) { --i; continue; }
        stringstream ss(line);
        int u, v; long long wll;
        if (!(ss >> u >> v)) continue;
        int w = 1;
        if (ss >> wll) w = (int)wll;
        ge.add_edge(u, v, w);
    }

    int Q;
    cin >> Q;
    getline(cin, rest);
    for (int qi = 0; qi < Q; ++qi) {
        string line;
        getline(cin, line);
        if (line.size() == 0) { --qi; continue; }
        stringstream ss(line);
        int type; ss >> type;
        if (type == 0) {
            int s; ss >> s;
            auto order = ge.bfs_lex(s);
            // print traversal
            for (size_t i = 0; i < order.size(); ++i) {
                if (i) cout << ' ';
                cout << order[i];
            }
            cout << "\n";
        } else if (type == 1) {
            int s; ss >> s;
            auto order = ge.dfs_lex(s);
            for (size_t i = 0; i < order.size(); ++i) {
                if (i) cout << ' ';
                cout << order[i];
            }
            cout << "\n";
        } else if (type == 2) {
            bool cyc;
            if (isDirected) cyc = ge.cycle_directed();
            else cyc = ge.cycle_undirected();
            cout << (cyc ? 1 : 0) << "\n";
        } else if (type == 3) {
            if (isDirected) {
                // unsupported; assignment says skip unsupported operations — print nothing (or you may print 0)
                // We'll print 0 to indicate "not bipartite" for directed (but safer to print 0)
                cout << 0 << "\n";
            } else {
                cout << (ge.bipartite_undirected() ? 1 : 0) << "\n";
            }
        } else if (type == 4) {
            cout << ge.count_connected_components() << "\n";
        } else if (type == 5) {
            int s, t; ss >> s >> t;
            long long dist = ge.dijkstra(s, t);
            cout << dist << "\n";
        } else if (type == 6) {
            if (!isDirected) {
                // For undirected, print each connected component as a line
                auto comps = ge.kosaraju_sccs();
                for (auto &c : comps) {
                    for (size_t i = 0; i < c.size(); ++i) {
                        if (i) cout << ' ';
                        cout << c[i];
                    }
                    cout << "\n";
                }
            } else {
                auto sccs = ge.kosaraju_sccs();
                for (auto &c : sccs) {
                    for (size_t i = 0; i < c.size(); ++i) {
                        if (i) cout << ' ';
                        cout << c[i];
                    }
                    cout << "\n";
                }
            }
        } else if (type == 7) {
            if (!isDirected) {
                // unsupported
                cout << "\n";
            } else {
                auto topo = ge.topological_sort_lex();
                if (topo.empty()) {
                    // not a DAG -> skip (print nothing or a blank line)
                    cout << "\n";
                } else {
                    for (size_t i = 0; i < topo.size(); ++i) {
                        if (i) cout << ' ';
                        cout << topo[i];
                    }
                    cout << "\n";
                }
            }
        } else if (type == 8) {
            int u, v; long long wll;
            if (!(ss >> u >> v)) continue;
            int w = 1;
            if (ss >> wll) w = (int)wll;
            ge.add_edge(u, v, w);
            // no explicit output requested for this operation
        } else if (type == 9) {
            int id = ge.add_vertex();
            cout << id << "\n";
        } else {
            // unknown - ignore
        }
    }
    return 0;
}
