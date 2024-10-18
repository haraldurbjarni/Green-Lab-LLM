#include <iostream>
#include <vector>
#include <fstream>
#include <climits>

using namespace std;

struct Edge {
  int source, destination, weight;
};

// Function to perform the Bellman-Ford algorithm
vector<int> bellmanFord(int V, int E, vector<Edge> &edges, int source) {
  // Initialize distances from the source to all other vertices as INFINITE
  vector<int> distance(V, INT_MAX);
  distance[source] = 0;

  // Relax all edges |V| - 1 times. A simple shortest path from src
  // to any other vertex can have at-most |V| - 1 edges
  for (int i = 1; i <= V - 1; i++) {
    for (int j = 0; j < E; j++) {
      int u = edges[j].source;
      int v = edges[j].destination;
      int weight = edges[j].weight;
      if (distance[u] != INT_MAX && distance[u] + weight < distance[v]) {
        distance[v] = distance[u] + weight;
      }
    }
  }

  // Check for negative-weight cycles. The above step guarantees
  // shortest distances if graph doesn't contain negative weight cycle.
  // If we get a shorter path, then there is a cycle.
  for (int i = 0; i < E; i++) {
    int u = edges[i].source;
    int v = edges[i].destination;
    int weight = edges[i].weight;
    if (distance[u] != INT_MAX && distance[u] + weight < distance[v]) {
      cout << "Graph contains negative weight cycle" << endl;
      return {}; // Return an empty vector to indicate an error
    }
  }

  return distance;
}

int main() {
  int V, E; // Number of vertices and edges
  vector<Edge> edges;

  // Read graph data from file
  ifstream graphFile("./Experiment/BFA/edges.txt");
  if (graphFile.is_open()) {
    graphFile >> V >> E;
    for (int i = 0; i < E; i++) {
      Edge edge;
      graphFile >> edge.source >> edge.destination >> edge.weight;
      edges.push_back(edge);
    }
    graphFile.close();
  } else {
    cout << "Unable to open edges.txt" << endl;
    return 1;
  }

  int source = 0; // Assuming source vertex is 0
  vector<int> distances = bellmanFord(V, E, edges, source);

  if (!distances.empty()) {
    cout << "Vertex\tDistance from Source" << endl;
    for (int i = 0; i < V; i++) {
      cout << i << "\t\t" << distances[i] << endl;
    }
  }

  return 0;
}
