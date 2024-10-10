#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

struct Item {
    int weight;
    int profit;
    double ratio;  // profit per unit weight
};

struct Node {
    int level;
    int profit;
    int weight;
    double bound;
};

class Knapsack {
private:
    int capacity;
    vector<Item> items;

    // Function to calculate upper bound at each node
    double calculateBound(Node node, int n) {
        if (node.weight >= capacity)
            return 0;

        double profitBound = node.profit;
        int j = node.level + 1;
        int totalWeight = node.weight;

        while (j < n && totalWeight + items[j].weight <= capacity) {
            totalWeight += items[j].weight;
            profitBound += items[j].profit;
            j++;
        }

        if (j < n)
            profitBound += (capacity - totalWeight) * items[j].ratio;

        return profitBound;
    }

public:
    Knapsack(int cap) : capacity(cap) {}

    void readData(const string& weightFile, const string& profitFile) {
        ifstream weightIn(weightFile);
        ifstream profitIn(profitFile);

        if (!weightIn.is_open() || !profitIn.is_open()) {
            cout << "Error opening files!" << endl;
            return;
        }

        int w, p;
        while (weightIn >> w && profitIn >> p) {
            items.push_back({w, p, static_cast<double>(p)/w});
        }

        weightIn.close();
        profitIn.close();

        // Sort items by profit/weight ratio in descending order
        sort(items.begin(), items.end(),
             [](const Item& a, const Item& b) { return a.ratio > b.ratio; });
    }

    int solve() {
        int n = items.size();
        queue<Node> Q;
        Node u, v;

        // Create root node
        u.level = -1;
        u.profit = u.weight = 0;
        Q.push(u);

        int maxProfit = 0;

        while (!Q.empty()) {
            u = Q.front();
            Q.pop();

            if (u.level == -1)
                v.level = 0;
            else if (u.level == n-1)
                continue;
            else
                v.level = u.level + 1;

            // Include current item
            v.weight = u.weight + items[v.level].weight;
            v.profit = u.profit + items[v.level].profit;

            if (v.weight <= capacity && v.profit > maxProfit)
                maxProfit = v.profit;

            v.bound = calculateBound(v, n);

            if (v.bound > maxProfit)
                Q.push(v);

            // Do not include current item
            v.weight = u.weight;
            v.profit = u.profit;
            v.bound = calculateBound(v, n);

            if (v.bound > maxProfit)
                Q.push(v);
        }

        return maxProfit;
    }
};

int main() {
    int capacity = 50;  // Set your desired capacity
    Knapsack knapsack(capacity);

    knapsack.readData("weight.txt", "profit.txt");

    int maxProfit = knapsack.solve();
    cout << "Maximum profit: " << maxProfit << endl;

    return 0;
}