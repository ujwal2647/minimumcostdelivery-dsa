// Minimum Cost Delivery Network (DSA2-P015)  -  C++ console version
//
// Goal    : connect all delivery hubs with the minimum total road cost.
// Data    : edge list (vector<Edge>) + DSU (Union-Find)      -> see core.h
// Method  : Kruskal's algorithm. Prim's algorithm is used to double-check the answer.
//
// Files   : main.cpp (menu, input, printing)   core.h (DSU, Kruskal, Prim, analysis)
// Compile : g++ main.cpp -o network
// Run     : network        (Windows: .\network.exe)
#include <bits/stdc++.h>
#include "core.h"
using namespace std;

// the network (starts empty, the user builds it) 
bool present[MAX_V];      // present[i] = hub i exists
vector<Edge> edges;       // all roads (working and failed)

// position of the road between a and b in the list, or -1
int findEdge(int u, int v) {
    for (int i = 0; i < (int)edges.size(); i++)
        if ((edges[i].u == u && edges[i].v == v) || (edges[i].u == v && edges[i].v == u)) return i;
    return -1;
}

bool validHub(int id) { return id >= 0 && id < MAX_V && present[id]; }

int countHubs() {
    int n = 0;
    for (int i = 0; i < MAX_V; i++) if (present[i]) n++;
    return n;
}

//safe input 
int readInt(const string& prompt) {
    while (true) {
        cout << prompt;
        string line;
        if (!getline(cin, line)) exit(0);                 // input finished
        try {
            size_t used;
            int x = stoi(line, &used);
            if (line.find_first_not_of(" \t\r", used) == string::npos) return x;
        } catch (...) {}
        cout << "  Please type a whole number.\n";
    }
}

string readText(const string& prompt) {
    cout << prompt;
    string s;
    getline(cin, s);
    return s;
}

//editing the network
void addHub() {
    int id = readInt("Hub ID (0-99): ");
    if (id < 0 || id >= MAX_V) cout << "  ID must be between 0 and 99.\n";
    else if (present[id])      cout << "  Hub already exists.\n";
    else { present[id] = true; cout << "  Hub " << id << " added.\n"; }
}

void removeHub() {
    int id = readInt("Hub ID to remove: ");
    if (!validHub(id)) { cout << "  No such hub.\n"; return; }
    present[id] = false;
    for (int i = (int)edges.size() - 1; i >= 0; i--)      // its roads go too
        if (edges[i].u == id || edges[i].v == id) edges.erase(edges.begin() + i);
    cout << "  Hub " << id << " and its roads removed.\n";
}

void addRoad() {
    int u = readInt("From hub: ");
    int v = readInt("To hub: ");
    int w = readInt("Road cost: ");
    if (!validHub(u) || !validHub(v))   cout << "  Both hubs must exist first.\n";
    else if (u == v)                    cout << "  A road needs two different hubs.\n";
    else if (w <= 0)                    cout << "  Cost must be a positive number.\n";
    else if (findEdge(u, v) != -1)      cout << "  That road already exists.\n";
    else if ((int)edges.size() >= MAX_E) cout << "  Maximum 500 roads reached.\n";
    else { edges.push_back({u, v, w, false}); cout << "  Road " << u << " - " << v << " added.\n"; }
}

void removeRoad() {
    int a = readInt("Hub A: ");
    int b = readInt("Hub B: ");
    int i = findEdge(a, b);
    if (i == -1) cout << "  Road not found.\n";
    else { edges.erase(edges.begin() + i); cout << "  Road removed.\n"; }
}

void toggleFailed() {
    int a = readInt("Hub A: ");
    int b = readInt("Hub B: ");
    int i = findEdge(a, b);
    if (i == -1) { cout << "  Road not found.\n"; return; }
    edges[i].failed = !edges[i].failed;
    cout << "  Road is now " << (edges[i].failed ? "FAILED (ignored)" : "working") << ".\n";
}

void changeCost() {
    int a = readInt("Hub A: ");
    int b = readInt("Hub B: ");
    int i = findEdge(a, b);
    if (i == -1) { cout << "  Road not found.\n"; return; }
    int w = readInt("New cost: ");
    if (w <= 0) cout << "  Cost must be a positive number.\n";
    else { edges[i].w = w; cout << "  Cost updated.\n"; }
}

// enter several roads in a row; hubs are created automatically
void bulkInput() {
    int m = readInt("How many roads do you want to enter? ");
    for (int i = 1; i <= m; i++) {
        cout << "Road " << i << ":\n";
        int u = readInt("  From hub: ");
        int v = readInt("  To hub: ");
        int w = readInt("  Cost: ");
        bool bad = u < 0 || u >= MAX_V || v < 0 || v >= MAX_V || u == v || w <= 0;
        if (bad || (int)edges.size() >= MAX_E || findEdge(u, v) != -1) {
            cout << "  Invalid or duplicate road, skipped.\n";
            continue;
        }
        present[u] = present[v] = true;
        edges.push_back({u, v, w, false});
    }
}

void clearAll() {
    string answer = readText("Remove ALL hubs and roads? (y/n): ");
    if (answer != "y" && answer != "Y") { cout << "  Cancelled.\n"; return; }
    for (int i = 0; i < MAX_V; i++) present[i] = false;
    edges.clear();
    cout << "  Network is empty again.\n";
}

//showing the network
void showGraph() {
    cout << "\nHubs (" << countHubs() << "): ";
    for (int i = 0; i < MAX_V; i++) if (present[i]) cout << i << " ";
    cout << "\nRoads (" << edges.size() << "):\n";
    if (edges.empty()) cout << "  (none)\n";
    for (const Edge& e : edges)
        cout << "  " << e.u << " - " << e.v << "   cost " << e.w << (e.failed ? "   [FAILED]" : "") << "\n";
}

// Kruskal + DSU. show = true prints every step.
void runKruskal(bool show) {
    vector<Edge> list;                                    // working roads only
    long long allCost = 0;
    for (const Edge& e : edges) if (!e.failed) { list.push_back(e); allCost += e.w; }
    if (list.empty()) { cout << "  There are no working roads yet.\n"; return; }
    sort(list.begin(), list.end(), edgeLess);

    DSU dsu;
    dsu.init();
    long long total = 0;
    int chosen = 0;

    cout << "\n--- Kruskal's algorithm ---\n";
    if (show) cout << "Roads sorted by cost. Every hub starts in its own set.\n";
    for (const Edge& e : list) {
        int ru = dsu.find(e.u), rv = dsu.find(e.v);
        bool joined = dsu.unite(e.u, e.v);
        if (joined) { total += e.w; chosen++; }

        if (!show) {
            if (joined) cout << "Selected road " << e.u << " - " << e.v << "  (cost " << e.w << ")\n";
            continue;
        }
        cout << "Road " << e.u << "-" << e.v << " (cost " << e.w << "): find(" << e.u << ")=" << ru
             << ", find(" << e.v << ")=" << rv << (joined ? "  -> different sets, SELECTED\n"
                                                           : "  -> same set (cycle), skipped\n");
        cout << "   set leader of each hub: ";
        for (int i = 0; i < MAX_V; i++) if (present[i]) cout << i << "->" << dsu.find(i) << "  ";
        cout << "\n";
    }
    // group the hubs by their set leader
    vector<vector<int>> groups(MAX_V);
    int comps = 0;
    for (int i = 0; i < MAX_V; i++) if (present[i]) {
        int r = dsu.find(i);
        if (groups[r].empty()) comps++;
        groups[r].push_back(i);
    }
    cout << "\nSelected roads : " << chosen << "\n";
    cout << "Total cost     : " << total << "\n";
    cout << "All roads cost : " << allCost << "  (network saves " << allCost - total << ")\n";
    cout << "Components     : " << comps << "\n";
    int n = 0;
    for (int r = 0; r < MAX_V; r++) if (!groups[r].empty()) {
        cout << "  Component " << ++n << ": ";
        for (int x : groups[r]) cout << x << " ";
        cout << "\n";
    }
    if (comps > 1) cout << "(Not every hub is connected, so this is a minimum spanning forest.)\n";
}
// table of the selected roads, with the total growing road by road
void showSelectedRoads() {
    MSTInfo k = kruskalMST(edges, present);
    if (k.chosen.empty()) { cout << "  No selected roads yet.\n"; return; }
    cout << "\n" << setw(4) << "No." << setw(10) << "Road" << setw(8) << "Cost" << setw(16) << "Running total\n";
    long long running = 0;
    for (size_t i = 0; i < k.chosen.size(); i++) {
        const Edge& e = k.chosen[i];
        running += e.w;
        string road = to_string(e.u) + " - " + to_string(e.v);
        cout << setw(4) << i + 1 << setw(10) << road << setw(8) << e.w << setw(15) << running << "\n";
    }
    cout << "Total cost: " << k.total << "    Components: " << k.components << "\n";
}
// put every failed road back into service
void restoreAllFailed() {
    int count = 0;
    for (Edge& e : edges) if (e.failed) { e.failed = false; count++; }
    cout << "  " << count << " failed road(s) restored.\n";
}
//extra analysis (uses core.h) 
void connectivityTest() {
    int a = readInt("Hub A: ");
    int b = readInt("Hub B: ");
    if (!validHub(a) || !validHub(b)) { cout << "  Both hubs must exist.\n"; return; }
    DSU dsu;
    dsu.init();
    for (const Edge& e : edges) if (!e.failed) dsu.unite(e.u, e.v);
    if (dsu.find(a) == dsu.find(b)) cout << "  Hubs " << a << " and " << b << " are CONNECTED.\n";
    else cout << "  Hubs " << a << " and " << b << " are NOT connected.\n";
}
void showStats() {
    Stats s = computeStats(edges, present);
    cout << "\n--- Network statistics ---\n";
    cout << "Hubs            : " << s.hubs << "  (isolated: " << s.isolatedHubs << ")\n";
    cout << "Roads           : " << s.roads << "  (failed: " << s.failedRoads << ")\n";
    if (s.roads - s.failedRoads > 0) {
        cout << "Cheapest road   : " << s.cheapest << "\n";
        cout << "Costliest road  : " << s.costliest << "\n";
        cout << "Average cost    : " << s.average << "\n";
    }
    if (s.busiestHub != -1)
        cout << "Busiest hub     : " << s.busiestHub << "  (" << s.degree[s.busiestHub] << " working roads)\n";
    cout << "Roads at each hub:\n";
    for (int i = 0; i < MAX_V; i++) if (present[i])
        cout << "  hub " << i << " : " << s.degree[i] << "\n";
}
// Kruskal (edge list + DSU) and Prim (adjacency list + heap) must give the same cost
void comparePrim() {
    if (countHubs() == 0) { cout << "  The network is empty.\n"; return; }
    MSTInfo k = kruskalMST(edges, present);
    int primComps = 0;
    long long p = primCost(edges, present, primComps);
    cout << "\n--- Kruskal vs Prim ---\n";
    cout << "Kruskal : cost " << k.total << ", components " << k.components << "\n";
    cout << "Prim    : cost " << p << ", components " << primComps << "\n";
    if (k.total == p && k.components == primComps)
        cout << "Both algorithms agree, so the answer is correct.\n";
    else
        cout << "They DIFFER - something is wrong!\n";
}
// For every selected road: what happens if it fails?
void showImpact() {
    vector<Impact> list = failureImpact(edges, present);
    if (list.empty()) { cout << "  No selected roads yet.\n"; return; }
    cout << "\n--- What if a selected road fails? ---\n";
    for (const Impact& im : list) {
        cout << "Road " << im.road.u << " - " << im.road.v << " (cost " << im.road.w << "): ";
        if (im.disconnects) cout << "CRITICAL - some hubs become unreachable\n";
        else cout << "network is rebuilt, costs " << im.extraCost << " more\n";
    }
}
//files
// Save format:  V E  /  hub IDs  /  E lines "u v w failed"
void saveGraph() {
    string name = readText("File name (example: graph.txt): ");
    ofstream out(name);
    if (!out) { cout << "  Cannot write that file.\n"; return; }
    out << countHubs() << " " << edges.size() << "\n";
    for (int i = 0; i < MAX_V; i++) if (present[i]) out << i << " ";
    out << "\n";
    for (const Edge& e : edges) out << e.u << " " << e.v << " " << e.w << " " << (e.failed ? 1 : 0) << "\n";
    cout << "  Saved to " << name << ".\n";
}
void loadGraph() {
    string name = readText("File name to load: ");
    ifstream in(name);
    int v, m;
    if (!in || !(in >> v >> m) || v < 0 || v > MAX_V || m < 0 || m > MAX_E) {
        cout << "  Cannot read that file.\n";
        return;
    }
    bool newPresent[MAX_V] = {false};                     // read into temporary copies first,
    vector<Edge> newEdges;                                // so a bad file cannot damage the network
    for (int i = 0; i < v; i++) {
        int id;
        if (!(in >> id) || id < 0 || id >= MAX_V) { cout << "  File has a bad hub list.\n"; return; }
        newPresent[id] = true;
    }
    for (int i = 0; i < m; i++) {
        int u, x, w, f;
        bool ok = (bool)(in >> u >> x >> w >> f);
        if (!ok || u < 0 || u >= MAX_V || x < 0 || x >= MAX_V || !newPresent[u] || !newPresent[x] || u == x || w <= 0) {
            cout << "  File has a bad road line.\n";
            return;
        }
        newEdges.push_back({u, x, w, f == 1});
    }
    for (int i = 0; i < MAX_V; i++) present[i] = newPresent[i];
    edges = newEdges;
    cout << "  Loaded " << v << " hubs and " << m << " roads.\n";
}

// a text report with the final answer, handy to hand in or paste into a document
void saveReport() {
    if (countHubs() == 0) { cout << "  The network is empty.\n"; return; }
    string name = readText("Report file name (example: report.txt): ");
    ofstream out(name);
    if (!out) { cout << "  Cannot write that file.\n"; return; }

    MSTInfo k = kruskalMST(edges, present);
    long long allCost = 0;
    for (const Edge& e : edges) if (!e.failed) allCost += e.w;
    out << "MINIMUM COST DELIVERY NETWORK - REPORT\n";
    out << "======================================\n";
    out << "Hubs  : " << countHubs() << "\n";
    out << "Roads : " << edges.size() << "\n\n";
    out << "Selected roads (Kruskal order):\n";
    for (const Edge& e : k.chosen) out << "  " << e.u << " - " << e.v << "   cost " << e.w << "\n";
    out << "\nTotal cost      : " << k.total << "\n";
    out << "All roads cost  : " << allCost << "  (saves " << allCost - k.total << ")\n";
    out << "Components      : " << k.components << "\n\n";
    out << "If a selected road fails:\n";
    for (const Impact& im : failureImpact(edges, present)) {
        out << "  " << im.road.u << " - " << im.road.v << " : ";
        if (im.disconnects) out << "CRITICAL (some hubs unreachable)\n";
        else out << "+" << im.extraCost << " cost\n";
    }
    cout << "  Report saved to " << name << ".\n";
}
// ---------- menu ----------
void showMenu() {
    cout << "\n=========== MINIMUM COST DELIVERY NETWORK ===========\n"
         << " 1. Add hub                 2. Remove hub\n"
         << " 3. Add road                4. Remove road\n"
         << " 5. Fail / restore road     6. Enter many roads\n"
         << " 7. Show network            8. Change road cost\n"
         << " 9. Run Kruskal (quick)    10. Run Kruskal (step by step)\n"
         << "11. Connectivity test      12. Network statistics\n"
         << "13. Kruskal vs Prim check  14. Road failure impact\n"
         << "15. Save network to file   16. Load network from file\n"
         << "17. Save report            18. Clear everything\n"
         << "19. Selected roads table   20. Restore all failed roads\n"
         << " 0. Exit\n";
}
int main() {
    cout << "Empty network. Add hubs (1) and roads (3), or enter many roads at once (6).\n";
    while (true) {
        showMenu();
        int c = readInt("Choice: ");
        if (c == 1) addHub();
        else if (c == 2) removeHub();
        else if (c == 3) addRoad();
        else if (c == 4) removeRoad();
        else if (c == 5) toggleFailed();
        else if (c == 6) bulkInput();
        else if (c == 7) showGraph();
        else if (c == 8) changeCost();
        else if (c == 9) runKruskal(false);
        else if (c == 10) runKruskal(true);
        else if (c == 11) connectivityTest();
        else if (c == 12) showStats();
        else if (c == 13) comparePrim();
        else if (c == 14) showImpact();
        else if (c == 15) saveGraph();
        else if (c == 16) loadGraph();
        else if (c == 17) saveReport();
        else if (c == 18) clearAll();
        else if (c == 19) showSelectedRoads();
        else if (c == 20) restoreAllFailed();
        else if (c == 0) { cout << "Goodbye!\n"; return 0; }
        else cout << "  Invalid choice.\n";
    }
}
