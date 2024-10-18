#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <limits>
#include <sstream> // Include this header file

// Structure to represent an edge
struct Edge {
    int source;
    int destination;
    int weight;
};

// Function to read the graph from a file
std::vector<Edge> readGraphFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        exit(1);
    }

    std::vector<Edge> graph;
    std::string line;

    // Read the number of vertices and edges
    std::getline(file, line);
    std::istringstream iss(line);
    int V, E;
    iss >> V >> E;

    // Read the edges
    while (E-- > 0) {
        std::getline(file, line);
        std::istringstream iss2(line);
        int source, destination, weight;
        iss2 >> source >> destination >> weight;

        // Create an edge
        graph.push_back({source, destination, weight});
    }

    return graph;
}

// Function to print the graph
void printGraph(const std::vector<Edge>& graph) {
    std::cout << "Graph:" << std::endl;
    for (const auto& edge : graph) {
        std::cout << "Source: " << edge.source << ", Destination: " << edge.destination << ", Weight: " << edge.weight << std::endl;
    }
}

// Function to calculate the shortest distances from a source vertex to all other vertices
std::vector<int> bellmanFord(const std::vector<Edge>& graph, int source) {
    std::vector<int> distances(graph.size(), std::numeric_limits<int>::max());
    distances[source] = 0;

    // Relax edges repeatedly
    for (int i = 0; i < graph.size() - 1; ++i) {
        for (const auto& edge : graph) {
            if (distances[edge.source]!= std::numeric_limits<int>::max() && distances[edge.source] + edge.weight < distances[edge.destination]) {
                distances[edge.destination] = distances[edge.source] + edge.weight;
            }
        }
    }

    return distances;
}

// Function to find the shortest path using Bellman-Ford
std::vector<std::vector<int>> findShortestPath(const std::vector<Edge>& graph, int source, int destination) {
    std::vector<int> distances = bellmanFord(graph, source);
    std::vector<std::vector<int>> shortestPaths;

    // Build the shortest paths
    for (int i = 0; i < graph.size(); ++i) {
        if (i!= source && i!= destination) {
            std::vector<int> path;
            int current = destination;
            while (current!= source) {
                path.push_back(current);
                current = graph[current].destination;
            }
            path.push_back(source);
            std::reverse(path.begin(), path.end());
            shortestPaths.push_back(path);
        }
    }

    return shortestPaths;
}

int main() {
    std::string filename = "./Experiment/BFA/edges.txt";
    std::vector<Edge> graph = readGraphFromFile(filename);
    int source = 0;
    int destination = graph.size() - 1;

    std::cout << "Graph:" << std::endl;
    printGraph(graph);

    std::vector<std::vector<int>> shortestPaths = findShortestPath(graph, source, destination);

    std::cout << "Shortest Paths:" << std::endl;
    for (const auto& path : shortestPaths) {
        for (int i = 0; i < path.size() - 1; ++i) {
            std::cout << path[i] << " -> " << path[i + 1] << std::endl;
        }
    }

    return 0;
}
