import random

# Define the possible characters
strings = ["A", "C", "T", "G"]


# Function to generate a random sequence
def generate_sequence(length):
    return "".join(random.choice(strings) for _ in range(length))


# Specify the lengths of the sequences
length_seq1 = 10000
length_seq2 = 10000

# Generate the sequences
seq1 = generate_sequence(length_seq1)
seq2 = generate_sequence(length_seq2)

# Write the sequences to the respective files
with open("seq1.txt", "w") as file1:
    file1.write(seq1)

with open("seq2.txt", "w") as file2:
    file2.write(seq2)

print("Sequences generated and written to seq1.txt and seq2.txt")
