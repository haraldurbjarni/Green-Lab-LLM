#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>

const int MATCH = 2;
const int MISMATCH = -1;
const int GAP_OPEN = -1;
const int GAP_EXTEND = -1;

void printMatrix(const std::vector<std::vector<int>>& matrix) {
    int rows = matrix.size();
    int cols = matrix[0].size();

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

std::vector<std::vector<int>> smithWaterman(const std::string& seq1, const std::string& seq2) {
    int len1 = seq1.length();
    int len2 = seq2.length();

    if (len1 != len2) {
        throw std::runtime_error("Sequences must have the same length.");
    }

    std::vector<std::vector<int>> matrix(len1 + 1, std::vector<int>(len2 + 1));

    for (int i = 0; i <= len1; ++i) {
        for (int j = 0; j <= len2; ++j) {
            if (i == 0 || j == 0) {
                matrix[i][j] = 0;
            } else if (seq1[i - 1] == seq2[j - 1]) {
                matrix[i][j] = matrix[i - 1][j - 1] + MATCH;
            } else {
                matrix[i][j] = std::max(matrix[i - 1][j], matrix[i][j - 1]) + MISMATCH;
            }
        }
    }

    int bestScore = matrix[len1][len2];
    int bestScoreIndex1 = len1;
    int bestScoreIndex2 = len2;

    for (int i = len1 - 1; i >= 0; --i) {
        for (int j = len2 - 1; j >= 0; --j) {
            if (matrix[i][j] > bestScore) {
                bestScore = matrix[i][j];
                bestScoreIndex1 = i;
                bestScoreIndex2 = j;
            }
        }
    }

    std::vector<int> alignment(bestScoreIndex1 + 1);
    int i = bestScoreIndex1;
    int j = bestScoreIndex2;

    while (i > 0 && j > 0) {
        if (seq1[i - 1] == seq2[j - 1]) {
            alignment[i] = j;
            --i;
            --j;
        } else if (matrix[i - 1][j] > matrix[i][j - 1]) {
            --i;
        } else {
            --j;
        }
    }

    std::string alignmentString;
    for (int index : alignment) {
        alignmentString += seq1[index - 1];
    }
    std::reverse(alignmentString.begin(), alignmentString.end());

    matrix[0][0] = bestScore;
    matrix[0].resize(len2 + 1);
    matrix.resize(len1 + 1);

    return matrix;
}
int main() {
    std::ifstream file1("./Experiment/SWA/seq1.txt");
    std::ifstream file2("./Experiment/SWA/seq2.txt");

    if (!file1.is_open() || !file2.is_open()) {
        std::cerr << "Error opening files." << std::endl;
        return 1;
    }

    std::string seq1;
    std::string seq2;

    std::string temp;
    while (std::getline(file1, temp)) {
        seq1 += temp;
    }
    while (std::getline(file2, temp)) {
        seq2 += temp;
    }

    auto matrix = smithWaterman(seq1, seq2);
    printMatrix(matrix);

    std::cout << "Best score: " << matrix[0][0] << std::endl;
    std::cout << "Alignment: " << std::endl;
    std::cout << seq1 << " " << seq2 << std::endl;
    std::cout << "------------------" << std::endl;
    for (char c1 : seq1) {
        char c2 = seq2[matrix[0][0] - matrix[0][matrix[0].size() - 1] + matrix[matrix.size() - 1].size() - matrix[matrix.size() - 1][matrix[0].size() - 1]];
        std::cout << c1 << c2;
    }
    std::cout << std::endl;

    file1.close();
    file2.close();

    return 0;
}
