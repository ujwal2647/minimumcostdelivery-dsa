# Minimum Cost Delivery Network (DSA2-P015)

**Live demo:** https://ujwal2647.github.io/minimumcostdelivery-dsa/MinCostDelivery/


## About
A logistics tool that connects delivery hubs with the minimum total road cost.
It uses an edge list and DSU (Union-Find) with Kruskal's algorithm to build a Minimum Spanning Tree.

## What you can do
- Add hubs and roads, or paste many roads at once
- See the selected roads, total cost and number of connected groups
- Watch Kruskal + DSU step by step
- Mark a road as failed and see how the network changes
- Test whether two hubs are connected
- Check what happens if a selected road fails

## How to use the live demo
1. Open the link above.
2. Paste roads into the box "Or paste many roads", one per line as `from to cost`:

0 1 7
0 3 5
1 2 8
1 3 9
1 4 7
2 4 5
3 4 15
3 5 6
4 5 8
4 6 9
5 6 11

4. Click "Add these roads". The total cost should be 39.

## C++ version
The files `main.cpp` and `core.h` in the `MinCostDelivery` folder are the C++ console version of the same project.
Keep both files in one folder and run:
g++ main.cpp -o network

## Files
- `MinCostDelivery/index.html` - the web screen
- `MinCostDelivery/main.cpp` - C++ menu program
- `MinCostDelivery/core.h` - DSU, Kruskal, Prim and analysis functions

##This project was developed as a DSA-based application, focusing on implementing core data structures and algorithms. AI was used as an assistance tool for frontend development, code formatting, and adding comments to improve code readability and maintainability, while the core logic and implementation were the primary focus of the project.
