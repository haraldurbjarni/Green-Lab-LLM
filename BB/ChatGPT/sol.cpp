#include <iostream>
#include <queue>
#include <vector>
#include <fstream>

using namespace std;

// Structure to represent an item (weight, value)
struct Item {
    int weight;
    int value;
};

// Structure to represent a node in the decision tree
struct Node {
    int level;     // Level of the node in the decision tree
    int profit;    // Total profit so far
    int weight;    // Total weight so far
    float bound;   // Upper bound on the maximum profit in the subtree
};

// Comparison function to sort items by value/weight ratio
bool compare(Item a, Item b) {
    double r1 = (double)a.value / a.weight;
    double r2 = (double)b.value / b.weight;
    return r1 > r2;
}

// Function to calculate the upper bound (using fractional knapsack approach)
float calculateBound(Node u, int n, int W, vector<Item> &items) {
    if (u.weight >= W) return 0; // If weight exceeds capacity, bound is 0

    int profit_bound = u.profit;
    int j = u.level + 1;
    int total_weight = u.weight;

    // Use the fractional knapsack idea to calculate the bound
    while (j < n && total_weight + items[j].weight <= W) {
        total_weight += items[j].weight;
        profit_bound += items[j].value;
        j++;
    }

    if (j < n)
        profit_bound += (W - total_weight) * (double)items[j].value / items[j].weight;

    return profit_bound;
}

// Branch and Bound algorithm for 0/1 Knapsack Problem
int knapsackBranchAndBound(vector<Item> &items, int W) {
    int n = items.size();

    // Sort items by value/weight ratio
    sort(items.begin(), items.end(), compare);

    // Create a queue to store nodes
    queue<Node> Q;

    // Starting with the root node (no items considered yet)
    Node u, v;
    u.level = -1;
    u.profit = 0;
    u.weight = 0;
    u.bound = calculateBound(u, n, W, items);

    // Initialize the queue with the root node
    Q.push(u);

    int max_profit = 0;

    // BFS to explore nodes
    while (!Q.empty()) {
        u = Q.front();
        Q.pop();

        // If we're at the last level, we stop
        if (u.level == n - 1) continue;

        // Consider taking the next item
        v.level = u.level + 1;
        v.weight = u.weight + items[v.level].weight;
        v.profit = u.profit + items[v.level].value;

        // If the weight is within capacity and the profit is greater than current max_profit
        if (v.weight <= W && v.profit > max_profit) {
            max_profit = v.profit;
        }

        // Calculate the bound (upper bound) for the node where we take the item
        v.bound = calculateBound(v, n, W, items);

        // If the bound is promising, push the node to the queue
        if (v.bound > max_profit) {
            Q.push(v);
        }

        // Now consider not taking the next item
        v.weight = u.weight;
        v.profit = u.profit;
        v.bound = calculateBound(v, n, W, items);

        // If this path is still promising, push it to the queue as well
        if (v.bound > max_profit) {
            Q.push(v);
        }
    }

    return max_profit;
}

// Function to read a list of integers from a file
vector<int> readFile(const string &filename) {
    vector<int> data;
    ifstream file(filename);
    if (!file) {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }

    int value;
    while (file >> value) {
        data.push_back(value);
    }

    file.close();
    return data;
}

int main() {
    // Reading weights and values from files
    vector<int> values = readFile("profit.txt");
    vector<int> weights = readFile("weights.txt");

    if (values.size() != weights.size()) {
        cerr << "Error: The number of values and weights must be the same." << endl;
        return 1;
    }

    int n = values.size();
    vector<Item> items(n);

    // Populate items with values and weights
    for (int i = 0; i < n; ++i) {
        items[i].value = values[i];
        items[i].weight = weights[i];
    }

    int W;
    cout << "Enter the capacity of the knapsack: ";
    cin >> W;

    int max_profit = knapsackBranchAndBound(items, W);

    cout << "Maximum profit: " << max_profit << endl;

    return 0;
}


