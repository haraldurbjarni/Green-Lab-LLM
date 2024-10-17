#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <list>

using namespace std;

const int INF = numeric_limits<int>::max();

struct Edge {
    int src, dest, weight;
};

void bellmanFord(int V, vector<Edge> edges, vector<int> &dist) {
    dist.assign(V, INF);
    dist[0] = 0;

    for (int i = 1; i < V - 1; ++i) {
        for (const Edge &e : edges) {
            int src = e.src;
            int dest = e.dest;
            int weight = e.weight;

            if (dist[src] != INF && dist[src] + weight < dist[dest]) {
                dist[dest] = dist[src] + weight;
            }
        }
    }

    for (const Edge &e : edges) {
        int src = e.src;
        int dest = e.dest;
        int weight = e.weight;

        if (dist[src] != INF && dist[src] + weight < dist[dest]) {
            cout << "Graph contains a negative weight cycle." << endl;
            return;
        }
    }
}

int main() {
    int V, E;
    ifstream in("./Experiment/BFA/edges.txt");
    in >> V >> E;

    vector<Edge> edges(E);
    for (auto &e : edges) {
        cin >> e.src >> e.dest >> e.weight;
    }

    vector<int> dist(V);
    bellmanFord(V, edges, dist);

    for (int i = 0; i < V; ++i) {
        cout << "Distance from source to vertex " << i << ": " << dist[i] << endl;
    }

    return 0;
}
