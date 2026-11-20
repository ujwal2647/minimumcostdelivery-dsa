# Minimum Cost Delivery Network (DSA2-P015)

Connect delivery hubs with the **minimum total road cost**.
Data structures: **edge list + DSU (Union-Find)**. Algorithm: **Kruskal's MST**.

## Files
| File | What it is |
|---|---|
| `index.html` | The screen (UI). Double-click to open in a browser. Nothing to install. |
| `main.cpp` | The C++ console program: menu, input, printing (about 400 lines). |
| `core.h` | The C++ data structures and algorithms: DSU, Kruskal, Prim, failure impact, statistics. |

## 1. Run the screen
Double-click `index.html`. Build your own network (nothing is pre-loaded):
1. Add hubs (IDs 0-99), or paste roads (`from to cost`, one per line; hubs are created automatically).
   A single number adds a hub. Add the word `failed` at the end of a line for a failed road.
2. Add roads with a cost. **Edit** a cost, mark a road **Fail** (ignored) or Restore, or Remove it.
3. The result updates after every change: total cost, selected roads (in the order Kruskal chose them),
   number of components, which hubs are in each group, and how much the network saves compared with
   building every road.
4. **Connectivity test**: are hub A and hub B connected? (DSU `find(a) == find(b)`, failed roads ignored)
5. **Show Kruskal step by step** (Back / Next / Auto-play): shows find(u), find(v), join or skip, and the parent[] array.
6. **What if a selected road fails?** checks every selected road: either CRITICAL (some hubs become unreachable)
   or how much more the rebuilt network costs. The **Statistics** card shows cheapest/costliest/average road and the busiest hub.
7. Your network is saved in the browser automatically (it is still there after a refresh).
   **Show my network as text** lets you copy it somewhere safe and paste it back later.

## 2. Run the C++ program
Keep `main.cpp` and `core.h` in the same folder.
```
g++ main.cpp -o network
network            (Mac/Linux: ./network)
```
Menu (20 options): add/remove hub, add/remove road, fail/restore road, enter many roads, show network,
change road cost, Kruskal quick / step by step, connectivity test, network statistics,
**Kruskal vs Prim check**, **road failure impact**, save/load network, save report, clear everything,
selected-roads table with running total, restore all failed roads.
(If Windows blocks the .exe, use the screen above, or paste both files into an online C++ compiler.)

Save file format: first line `V E`, second line the hub IDs, then `E` lines of `u v w failed` (failed is 0 or 1).

## How it works
1. Ignore failed roads, sort the rest by cost.
2. Every hub starts in its own set (`parent[i] = i`).
3. For each road (u, v): if `find(u) != find(v)` -> `unite` them and select the road; otherwise it would form a cycle, so skip it.
4. Total cost = sum of selected roads. Number of sets left = number of connected components
   (more than 1 means some hubs cannot be reached, so the answer is a minimum spanning forest).

## Why these data structures
- **Edge list (`vector<Edge>`)**: Kruskal only needs the roads sorted by cost. At most 500 roads.
- **DSU with path compression + union by rank**: cycle check in almost constant time, and it gives the connected components for free.
- **Adjacency list + min-heap (priority_queue)**: used only by Prim's algorithm, which recomputes the answer in a
  different way. If Kruskal and Prim agree, the result is correct (menu option 13).
- Complexity: Kruskal O(E log E) for sorting plus O(E x alpha(V)) for the DSU work. Prim O(E log V).
- Failure impact runs Kruskal once for each selected road: O(V x E log E), fine for 100 hubs and 500 roads.

## Limits and checks
Up to 100 hubs (IDs 0-99) and 500 roads. Only whole numbers are accepted, costs must be positive, and self-loops or duplicate roads are rejected.

## Note
`index.html` re-implements the same Kruskal + DSU logic in JavaScript so that it runs by double-click. `main.cpp` + `core.h` are the C++ version of the same algorithm.
