

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

struct Item {
    int id;
    int weight;
    int profit;
};

bool compare(const Item& a, const Item& b) {
    return a.profit > b.profit;
}

class Knapsack {
public:
    Knapsack(int capacity) : capacity(capacity) {}

    void readData(const string& weights_filename, const string& profits_filename) {
        ifstream weights_input(weights_filename);
        ifstream profits_input(profits_filename);

        if (!weights_input.is_open() || !profits_input.is_open()) {
            cerr << "Error opening file(s)." << endl;
            exit(1);
        }

        Item item;
        while (weights_input >> item.weight && profits_input >> item.profit) {
            items.push_back(item);
        }
        weights_input.close();
        profits_input.close();
    }

    void solve() {
        sort(items.begin(), items.end(), compare);
        solve(0, 0, 0);
        printSolution();
    }

private:
    void solve(int index, int currentWeight, int remainingCapacity) {
        if (index == items.size() || remainingCapacity < currentWeight) {
            return;
        }

        if (remainingCapacity >= currentWeight) {
            int newProfit = items[index].profit;
            solve(index + 1, currentWeight + items[index].weight, remainingCapacity - items[index].weight);
        }

        int notTakenProfit = items[index].profit;
        solve(index + 1, currentWeight, remainingCapacity);

        if (bound > notTakenProfit) {
            bound = notTakenProfit;
            bestSolution.clear();
        }

        if (remainingCapacity >= currentWeight && bound > (notTakenProfit + bound)) {
            bestSolution.push_back(items[index]);
            solve(index + 1, currentWeight + items[index].weight, remainingCapacity - items[index].weight);
        }
    }

    void printSolution() {
        cout << "Best solution:" << endl;
        for (const auto& item : bestSolution) {
            cout << item.id << " ";
        }
        cout << endl << "Total profit: " << bound << endl;
    }

    int capacity;
    vector<Item> items;
    vector<Item> bestSolution;
    int bound = numeric_limits<int>::min();
};

int main() {
    Knapsack knapsack(50);
    knapsack.readData("./Experiment/BB/weight.txt", "./Experiment/BB/profit.txt");
    knapsack.solve();
    return 0;
}
