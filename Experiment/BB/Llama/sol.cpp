#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>

// Structure to represent a node in the search tree
struct Node {
    int value;
    int profit;
    int weight;
    int index;
};

// Function to read weights from a file
std::vector<int> readWeights(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        exit(1);
    }

    std::vector<int> weights;
    std::string line;
    while (std::getline(file, line)) {
        weights.push_back(std::stod(line));
    }
    file.close();

    return weights;
}

// Function to read profits from a file
std::vector<int> readProfits(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        exit(1);
    }

    std::vector<int> profits;
    std::string line;
    while (std::getline(file, line)) {
        profits.push_back(std::stod(line));
    }
    file.close();

    return profits;
}

// Function to calculate the profit of a node
int calculateProfit(const Node& node, const std::vector<int>& weights, const std::vector<int>& profits) {
    if (node.weight == 0) {
        return 0;
    }

    int maxProfit = 0;
    for (int i = 0; i < weights.size(); ++i) {
        int profit = profits[i] - weights[i] * node.value;
        if (profit > maxProfit) {
            maxProfit = profit;
        }
    }

    return maxProfit;
}

// Function to perform the Branch and Bound algorithm
Node branchAndBound(const std::vector<int>& weights, const std::vector<int>& profits) {
    std::vector<Node> nodes;
    nodes.push_back(Node{0, 0, weights[0], 0});

    while (true) {
        std::vector<Node> newNodes;
        for (const auto& node : nodes) {
            if (node.value == 0) {
                continue;
            }

            for (int i = 0; i < weights.size(); ++i) {
                int profit = calculateProfit(node, weights, profits);
                if (profit > node.profit) {
                    newNodes.push_back(Node{profit, profit + weights[i] * node.value, weights[i], i});
                }
            }
        }

        if (nodes.empty()) {
            return nodes.back();
        }

        nodes = newNodes;
    }
}

// Function to print the results
void printResults(const Node& result) {
    std::cout << "Weight: " << result.weight << std::endl;
    std::cout << "Value: " << result.value << std::endl;
    std::cout << "Profit: " << result.profit << std::endl;
    std::cout << "Index: " << result.index << std::endl;
}

int main() {
    std::string filenameWeight = "../weight.txt";
    std::string filenameProfit = "../profit.txt";

    std::vector<int> weights = readWeights(filenameWeight);
    std::vector<int> profits = readProfits(filenameProfit);

    Node result = branchAndBound(weights, profits);

    printResults(result);

    return 0;
}


