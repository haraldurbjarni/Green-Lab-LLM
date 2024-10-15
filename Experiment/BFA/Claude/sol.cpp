#include <iostream>
#include <vector>
#include <fstream>
#include <climits>
#include <string>

struct Edge {
    int source;
    int destination;
    int weight;
};

class Graph {
private:
    int V, E;
    std::vector<Edge> edges;
    std::vector<int> distances;

public:
    Graph(const std::string& filename) {
        readGraphFromFile(filename);
    }

    void readGraphFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }

        file >> V >> E;

        for (int i = 0; i < E; i++) {
            Edge edge;
            file >> edge.source >> edge.destination >> edge.weight;
            edges.push_back(edge);
        }

        file.close();
    }

    bool bellmanFord(int source) {
        // Step 1: Initialize distances
        distances.resize(V, INT_MAX);
        distances[source] = 0;

        // Step 2: Relax all edges V-1 times
        for (int i = 1; i <= V - 1; i++) {
            for (const Edge& edge : edges) {
                int u = edge.source;
                int v = edge.destination;
                int weight = edge.weight;

                if (distances[u] != INT_MAX &&
                    distances[u] + weight < distances[v]) {
                    distances[v] = distances[u] + weight;
                }
            }
        }

        // Step 3: Check for negative-weight cycles
        for (const Edge& edge : edges) {
            int u = edge.source;
            int v = edge.destination;
            int weight = edge.weight;

            if (distances[u] != INT_MAX &&
                distances[u] + weight < distances[v]) {
                return false; // Graph contains negative weight cycle
            }
        }

        return true;
    }

    void printSolution() {
        std::cout << "Vertex\tDistance from Source\n";
        for (int i = 0; i < V; i++) {
            std::cout << i << "\t";
            if (distances[i] == INT_MAX)
                std::cout << "INF";
            else
                std::cout << distances[i];
            std::cout << std::endl;
        }
    }

    int getNumVertices() const { return V; }
};

int main() {
    std::string filename = "../edges.txt";
    Graph graph(filename);

    int source = 0; // Starting vertex

    if (graph.getNumVertices() > 0) {
        std::cout << "Running Bellman-Ford algorithm from vertex " << source << std::endl;

        if (graph.bellmanFord(source)) {
            std::cout << "No negative weight cycles found.\n" << std::endl;
            graph.printSolution();
        } else {
            std::cout << "Negative weight cycle detected!" << std::endl;
        }
    } else {
        std::cout << "Graph is empty or file could not be read properly." << std::endl;
    }

    return 0;
}
