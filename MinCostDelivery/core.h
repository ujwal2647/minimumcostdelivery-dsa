// core.h  -  data structures and algorithms for the Minimum Cost Delivery Network
//
//   DSU (Union-Find)  : cycle check and connected components
//   Kruskal           : minimum spanning tree (edge list + DSU)
//   Prim              : second algorithm (adjacency list + min-heap) used to double-check Kruskal
//   failureImpact     : "what happens if a selected road fails?"
//   computeStats      : simple numbers about the network
//
// main.cpp includes this file and takes care of the menu, input and printing.

#ifndef CORE_H
#define CORE_H

#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

const int MAX_V = 100;   // hub IDs are 0..99
const int MAX_E = 500;   // at most 500 roads

struct Edge { int u, v, w; bool failed; };

//DSU (Union-Find)
struct DSU {
    int parent[MAX_V];
    int rnk[MAX_V];

    void init() {                         // every hub starts in its own set
        for (int i = 0; i < MAX_V; i++) { parent[i] = i; rnk[i] = 0; }
    }
    int find(int x) {                     // path compression
        if (parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    }
    bool unite(int a, int b) {            // union by rank; false = already in the same set
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (rnk[a] < rnk[b]) swap(a, b);
        parent[b] = a;
        if (rnk[a] == rnk[b]) rnk[a]++;
        return true;
    }
};

// cheaper roads first; equal costs are ordered by hub number so results are always the same
inline bool edgeLess(const Edge& a, const Edge& b) {
    if (a.w != b.w) return a.w < b.w;
    if (a.u != b.u) return a.u < b.u;
    return a.v < b.v;
}

//Kruskal (no printing)
struct MSTInfo {
    vector<Edge> chosen;      // selected roads, in the order Kruskal picked them
    long long total = 0;      // total cost of the selected roads
    int components = 0;       // number of connected groups of hubs
};

inline MSTInfo kruskalMST(const vector<Edge>& edges, const bool* present) {
    MSTInfo info;
    vector<Edge> list;                                    // only working roads
    for (const Edge& e : edges) if (!e.failed) list.push_back(e);
    sort(list.begin(), list.end(), edgeLess);

    DSU dsu;
    dsu.init();
    for (const Edge& e : list) {
        if (dsu.unite(e.u, e.v)) {
            info.chosen.push_back(e);
            info.total += e.w;
        }
    }
    for (int i = 0; i < MAX_V; i++)                       
        if (present[i] && dsu.find(i) == i) info.components++;
    return info;
}


inline long long primCost(const vector<Edge>& edges, const bool* present, int& components) {
    vector<pair<int, int>> adj[MAX_V];                   
    for (const Edge& e : edges) {
        if (e.failed) continue;
        adj[e.u].push_back({e.v, e.w});
        adj[e.v].push_back({e.u, e.w});
    }

    bool seen[MAX_V] = {false};
    long long total = 0;
    components = 0;

    for (int start = 0; start < MAX_V; start++) {
        if (!present[start] || seen[start]) continue;
        components++;                                     // a new group begins here
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
        pq.push({0, start});                              // (cost to reach, hub)
        while (!pq.empty()) {
            pair<int, int> top = pq.top();
            pq.pop();
            int cost = top.first, x = top.second;
            if (seen[x]) continue;
            seen[x] = true;
            total += cost;
            for (const pair<int, int>& nb : adj[x])
                if (!seen[nb.first]) pq.push({nb.second, nb.first});
        }
    }
    return total;
}

//failure impact
struct Impact {
    Edge road;                // a selected road
    bool disconnects;         // true  = if it fails, some hubs become unreachable
    long long extraCost;      // false = the network gets this much more expensive
};

inline vector<Impact> failureImpact(const vector<Edge>& edges, const bool* present) {
    MSTInfo base = kruskalMST(edges, present);
    vector<Impact> result;
    for (const Edge& c : base.chosen) {
        vector<Edge> copy = edges;                        // pretend this road fails
        for (Edge& e : copy)
            if (e.u == c.u && e.v == c.v) e.failed = true;
        MSTInfo alt = kruskalMST(copy, present);
        Impact im;
        im.road = c;
        im.disconnects = alt.components > base.components;
        im.extraCost = im.disconnects ? 0 : alt.total - base.total;
        result.push_back(im);
    }
    return result;
}

//statistics 
struct Stats {
    int hubs = 0, roads = 0, failedRoads = 0;
    int degree[MAX_V] = {0};          // working roads at each hub
    int busiestHub = -1;              // hub with the most working roads
    int isolatedHubs = 0;             // hubs with no working road
    int cheapest = 0, costliest = 0;  // cost of the cheapest / most expensive working road
    double average = 0;               // average cost of a working road
};

inline Stats computeStats(const vector<Edge>& edges, const bool* present) {
    Stats s;
    long long sum = 0;
    int working = 0;
    for (int i = 0; i < MAX_V; i++) if (present[i]) s.hubs++;
    s.roads = (int)edges.size();
    for (const Edge& e : edges) {
        if (e.failed) { s.failedRoads++; continue; }
        s.degree[e.u]++;
        s.degree[e.v]++;
        if (working == 0 || e.w < s.cheapest) s.cheapest = e.w;
        if (working == 0 || e.w > s.costliest) s.costliest = e.w;
        sum += e.w;
        working++;
    }
    if (working > 0) s.average = (double)sum / working;
    for (int i = 0; i < MAX_V; i++) {
        if (!present[i]) continue;
        if (s.degree[i] == 0) s.isolatedHubs++;
        if (s.busiestHub == -1 || s.degree[i] > s.degree[s.busiestHub]) s.busiestHub = i;
    }
    return s;
}

#endif
