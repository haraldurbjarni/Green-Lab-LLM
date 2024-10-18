#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

// Define the scoring matrix
const int MATCH = 2;
const int MISMATCH = -1;
const int GAP = -1;

// Function to read a sequence from a file
std::string readSequence(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        exit(1);
    }

    std::string sequence;
    std::string line;
    while (std::getline(file, line)) {
        sequence += line;
    }
    file.close();

    return sequence;
}

// Function to perform the Smith-Waterman algorithm
int smithWaterman(const std::string& seq1, const std::string& seq2) {
    int m = seq1.length();
    int n = seq2.length();

    // Create a scoring matrix
    std::vector<std::vector<int>> score(m + 1, std::vector<int>(n + 1, 0));

    // Initialize the first row and column
    for (int i = 0; i <= m; ++i) {
        score[i][0] = GAP * i;
    }
    for (int j = 0; j <= n; ++j) {
        score[0][j] = GAP * j;
    }

    // Fill in the rest of the scoring matrix
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            int match = score[i - 1][j - 1] + (seq1[i - 1] == seq2[j - 1]? MATCH : MISMATCH);
            int deleteScore = score[i - 1][j] + GAP;
            int insertScore = score[i][j - 1] + GAP;

            score[i][j] = std::max({match, deleteScore, insertScore});
        }
    }

    // Find the maximum score
    int maxScore = 0;
    int maxScorePos = 0;
    for (int i = 0; i <= m; ++i) {
        for (int j = 0; j <= n; ++j) {
            if (score[i][j] > maxScore) {
                maxScore = score[i][j];
                maxScorePos = i;
            }
        }
    }

    // Reconstruct the optimal alignment
    std::string alignment;
    int i = maxScorePos;
    int j = maxScorePos;
    while (i > 0 || j > 0) {
        if (i > 0 && j > 0 && seq1[i - 1] == seq2[j - 1]) {
            alignment.push_back(seq1[i - 1]);
            --i;
            --j;
        } else if (score[i][j] == score[i - 1][j] + GAP) {
            alignment.push_back('-');
            --i;
        } else {
            alignment.push_back(seq2[j - 1]);
            --j;
        }
    }

    // Reverse the alignment
    std::reverse(alignment.begin(), alignment.end());

    return maxScore;
}

int main() {
    std::string seq1 = readSequence("./Experiment/SWA/seq1.txt");
    std::string seq2 = readSequence("./Experiment/SWA/seq2.txt");

    int score = smithWaterman(seq1, seq2);
    std::cout << "Maximum score: " << score << std::endl;

    return 0;
}
