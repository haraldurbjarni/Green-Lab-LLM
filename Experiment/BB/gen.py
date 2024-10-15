import random


# Function to generate random weights and profits and save them to files
def generate_knapsack_files(num_items, weight_file, profit_file):
    # Generate random weights and profits for the items
    weights = [
        random.randint(1, 20) for _ in range(num_items)
    ]  # Weights between 1 and 20
    profits = [
        random.randint(10, 100) for _ in range(num_items)
    ]  # Profits between 10 and 100

    # Save the weights to the weight_file
    with open(weight_file, "w") as wf:
        for weight in weights:
            wf.write(f"{weight}\n")

    # Save the profits to the profit_file
    with open(profit_file, "w") as pf:
        for profit in profits:
            pf.write(f"{profit}\n")

    print(
        f"Generated {num_items} weights and profits in '{weight_file}' and '{profit_file}'"
    )


# Parameters
num_items = 500  # Number of items to generate
weight_file = "weight.txt"
profit_file = "profit.txt"

# Generate the files
generate_knapsack_files(num_items, weight_file, profit_file)
