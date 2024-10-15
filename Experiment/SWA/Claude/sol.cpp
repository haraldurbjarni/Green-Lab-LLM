#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

// Scoring parameters
const int MATCH_SCORE = 2;
const int MISMATCH_PENALTY = -1;
const int GAP_PENALTY = -1;

struct Cell {
    int score;
    int i;
    int j;

    Cell(int s = 0, int i_val = -1, int j_val = -1)
        : score(s), i(i_val), j(j_val) {}
};

// Function to read sequence from file
std::string readSequenceFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string sequence;

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return "";
    }

    std::getline(file, sequence);
    file.close();

    return sequence;
}

std::vector<std::vector<Cell>> smith_waterman(const std::string& seq1, const std::string& seq2) {
    int m = seq1.length();
    int n = seq2.length();

    // Initialize the scoring matrix
    std::vector<std::vector<Cell>> matrix(m + 1, std::vector<Cell>(n + 1));

    // Fill the matrix
    int max_score = 0;
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            // Calculate match/mismatch score
            int match = matrix[i-1][j-1].score +
                        (seq1[i-1] == seq2[j-1] ? MATCH_SCORE : MISMATCH_PENALTY);

            // Calculate scores from different directions
            int delete_gap = matrix[i-1][j].score + GAP_PENALTY;
            int insert_gap = matrix[i][j-1].score + GAP_PENALTY;

            // Take the maximum score
            int max_val = std::max({0, match, delete_gap, insert_gap});

            if (max_val == 0) {
                matrix[i][j] = Cell(0, -1, -1);
            } else if (max_val == match) {
                matrix[i][j] = Cell(max_val, i-1, j-1);
            } else if (max_val == delete_gap) {
                matrix[i][j] = Cell(max_val, i-1, j);
            } else {
                matrix[i][j] = Cell(max_val, i, j-1);
            }

            max_score = std::max(max_score, max_val);
        }
    }

    return matrix;
}

void traceback(const std::vector<std::vector<Cell>>& matrix,
               const std::string& seq1, const std::string& seq2) {
    // Find the highest scoring cell
    int max_i = 0, max_j = 0, max_score = 0;
    for (size_t i = 0; i < matrix.size(); i++) {
        for (size_t j = 0; j < matrix[0].size(); j++) {
            if (matrix[i][j].score > max_score) {
                max_score = matrix[i][j].score;
                max_i = i;
                max_j = j;
            }
        }
    }

    if (max_score == 0) {
        std::cout << "No alignment found.\n";
        return;
    }

    // Traceback from highest scoring cell
    std::string align1, align2;
    int curr_i = max_i, curr_j = max_j;

    while (curr_i != -1 && curr_j != -1 && matrix[curr_i][curr_j].score > 0) {
        int next_i = matrix[curr_i][curr_j].i;
        int next_j = matrix[curr_i][curr_j].j;

        if (next_i == curr_i - 1 && next_j == curr_j - 1) {
            align1 = seq1[curr_i - 1] + align1;
            align2 = seq2[curr_j - 1] + align2;
        } else if (next_i == curr_i - 1) {
            align1 = seq1[curr_i - 1] + align1;
            align2 = '-' + align2;
        } else {
            align1 = '-' + align1;
            align2 = seq2[curr_j - 1] + align2;
        }

        curr_i = next_i;
        curr_j = next_j;
    }

    std::cout << "Alignment score: " << max_score << "\n";
    std::cout << "Sequence 1: " << align1 << "\n";
    std::cout << "Sequence 2: " << align2 << "\n";
}

int main() {
    std::string seq1 = readSequenceFromFile("../seq1.txt");
    std::string seq2 = readSequenceFromFile("../seq2.txt");

    if (seq1.empty() || seq2.empty()) {
        std::cerr << "Error: One or both sequence files are empty or couldn't be read." << std::endl;
        return 1;
    }

    std::cout << "Sequence 1: " << seq1 << "\n";
    std::cout << "Sequence 2: " << seq2 << "\n\n";

    auto matrix = smith_waterman(seq1, seq2);
    traceback(matrix, seq1, seq2);

    return 0;
}
