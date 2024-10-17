

#include <iostream>
#include <vector>
#include <queue>
#include <limits.h>
#include <fstream>

using namespace std;

// Structure to represent a node in the search tree
struct Node {
  int level; // Current level in the search tree
  int profit; // Profit of the current node
  int weight; // Weight of the current node
  int bound; // Upper bound on the profit
  vector<int> items; // Items included in the current node
};

// Function to calculate the upper bound on the profit
int bound(Node u, int n, int W, vector<int> &profit, vector<int> &weight) {
  int j, k;
  int totweight;
  int result;

  if (u.weight >= W) {
    return 0;
  } else {
    result = u.profit;
    j = u.level + 1;
    totweight = u.weight;
    while ((j < n) && (totweight + weight[j] <= W)) {
      totweight = totweight + weight[j];
      result = result + profit[j];
      j++;
    }
    k = j;
    if (k < n) {
      result = result + (W - totweight) * profit[k] / weight[k];
    }
    return result;
  }
}

// Function to perform the branch and bound algorithm
int knapsack(int W, vector<int> &profit, vector<int> &weight, int n) {
  queue<Node> Q;
  Node u, v;
  int maxProfit = 0;

  u.level = -1;
  u.profit = u.weight = 0;
  Q.push(u);

  while (!Q.empty()) {
    u = Q.front();
    Q.pop();

    if (u.level == -1) {
      v.level = 0;
    } else if (u.level != n - 1) {
      v.level = u.level + 1;
    }

    v.weight = u.weight + weight[v.level];
    v.profit = u.profit + profit[v.level];
    v.items = u.items;
    v.items.push_back(v.level);

    if (v.weight <= W && v.profit > maxProfit) {
      maxProfit = v.profit;
    }
    v.bound = bound(v, n, W, profit, weight);

    if (v.bound > maxProfit) {
      Q.push(v);
    }

    v.weight = u.weight;
    v.profit = u.profit;
    v.items = u.items;
    v.bound = bound(v, n, W, profit, weight);

    if (v.bound > maxProfit) {
      Q.push(v);
    }
  }

  return maxProfit;
}

int main() {
  int W; // Maximum weight capacity of the knapsack
  vector<int> profit, weight;
  int n; // Number of items

  // Read profit from "profit.txt"
  ifstream profitFile("./Experiment/BB/profit.txt");
  if (profitFile.is_open()) {
    int p;
    while (profitFile >> p) {
      profit.push_back(p);
    }
    profitFile.close();
  } else {
    cout << "Unable to open profit.txt" << endl;
    return 1;
  }

  // Read weight from "weight.txt"
  ifstream weightFile("./Experiment/BB/weight.txt");
  if (weightFile.is_open()) {
    int w;
    while (weightFile >> w) {
      weight.push_back(w);
    }
    weightFile.close();
  } else {
    cout << "Unable to open weight.txt" << endl;
    return 1;
  }

  // Check if the number of profits and weights match
  if (profit.size() != weight.size()) {
    cout << "Error: Number of profits and weights do not match." << endl;
    return 1;
  }

  n = profit.size();

  W = 50;

  int maxProfit = knapsack(W, profit, weight, n);

  cout << "Maximum profit: " << maxProfit << endl;

  return 0;
}
