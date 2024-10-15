#include <iostream>
#include <vector>
#include <fstream>
#include <climits>

using namespace std;

// Structure to represent an edge
struct Edge {
    int src, dest, weight;
};

// Function to perform the Bellman-Ford algorithm
void bellmanFord(vector<Edge> &edges, int V, int E, int src) {
    // Initialize distances to all vertices as infinite and src distance as 0
    vector<int> dist(V, INT_MAX);
    dist[src] = 0;

    // Relax all edges |V| - 1 times
    for (int i = 1; i <= V - 1; i++) {
        for (int j = 0; j < E; j++) {
            int u = edges[j].src;
            int v = edges[j].dest;
            int weight = edges[j].weight;
            if (dist[u] != INT_MAX && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
            }
        }
    }

    // Check for negative-weight cycles
    for (int j = 0; j < E; j++) {
        int u = edges[j].src;
        int v = edges[j].dest;
        int weight = edges[j].weight;
        if (dist[u] != INT_MAX && dist[u] + weight < dist[v]) {
            cout << "Graph contains negative weight cycle!" << endl;
            return;
        }
    }

    // Print the result
    cout << "Vertex Distance from Source " << src << ":" << endl;
    for (int i = 0; i < V; i++) {
        cout << "Vertex " << i << ": " << dist[i] << endl;
    }
}

// Function to read edges from a file
vector<Edge> readEdgesFromFile(const string &filename, int &V, int &E) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }

    vector<Edge> edges;
    file >> V >> E; // First line should contain the number of vertices and edges

    int u, v, w;
    while (file >> u >> v >> w) {
        edges.push_back({u, v, w});
    }

    file.close();
    return edges;
}

int main() {
    // Number of vertices and edges (will be read from file)
    int V, E;

    // Read edges from file
    string filename = "../edges.txt"; // Your file name
    vector<Edge> edges = readEdgesFromFile(filename, V, E);

    // Perform Bellman-Ford algorithm
    bellmanFord(edges, V, E, 0); // 0 is the source vertex

    return 0;
}
