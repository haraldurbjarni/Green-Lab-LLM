#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>

using namespace std;

// Scoring parameters
const int match_score = 2;
const int mismatch_penalty = -1;
const int gap_penalty = -1;

struct AlignmentResult {
    int score;
    string alignment1;
    string alignment2;
};

AlignmentResult smithWaterman(const string& seq1, const string& seq2) {
    int m = seq1.length();
    int n = seq2.length();

    // Initialize scoring matrix
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));

    // Fill the scoring matrix
    int maxScore = 0;
    int maxScoreRow = 0;
    int maxScoreCol = 0;
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            int match = dp[i - 1][j - 1] + (seq1[i - 1] == seq2[j - 1] ? match_score : mismatch_penalty);
            int gap1 = dp[i - 1][j] + gap_penalty;
            int gap2 = dp[i][j - 1] + gap_penalty;

            dp[i][j] = max({0, match, gap1, gap2});

            if (dp[i][j] > maxScore) {
                maxScore = dp[i][j];
                maxScoreRow = i;
                maxScoreCol = j;
            }
        }
    }

    // Traceback to construct the alignment
    string alignment1 = "";
    string alignment2 = "";
    int i = maxScoreRow;
    int j = maxScoreCol;
    while (i > 0 || j > 0) {
        if (i > 0 && j > 0 && dp[i][j] == dp[i - 1][j - 1] + (seq1[i - 1] == seq2[j - 1] ? match_score : mismatch_penalty)) {
            alignment1 = seq1[i - 1] + alignment1;
            alignment2 = seq2[j - 1] + alignment2;
            --i;
            --j;
        } else if (i > 0 && dp[i][j] == dp[i - 1][j] + gap_penalty) {
            alignment1 = seq1[i - 1] + alignment1;
            alignment2 = "-" + alignment2;
            --i;
        } else if (j > 0 && dp[i][j] == dp[i][j - 1] + gap_penalty) {
            alignment1 = "-" + alignment1;
            alignment2 = seq2[j - 1] + alignment2;
            --j;
        } else {
            break; // Reached the beginning of the local alignment
        }
    }

    AlignmentResult result;
    result.score = maxScore;
    result.alignment1 = alignment1;
    result.alignment2 = alignment2;
    return result;
}

int main() {
    string seq1, seq2;

    // Read seq1 from "seq1.txt"
    ifstream seq1File("../seq1.txt");
    if (seq1File.is_open()) {
        getline(seq1File, seq1); // Read the entire line as seq1
        seq1File.close();
    } else {
        cout << "Unable to open seq1.txt" << endl;
        return 1;
    }

    // Read seq2 from "seq2.txt"
    ifstream seq2File("../seq2.txt");
    if (seq2File.is_open()) {
        getline(seq2File, seq2); // Read the entire line as seq2
        seq2File.close();
    } else {
        cout << "Unable to open seq2.txt" << endl;
        return 1;
    }

    AlignmentResult result = smithWaterman(seq1, seq2);

    cout << "Alignment Score: " << result.score << endl;
    cout << "Alignment 1:     " << result.alignment1 << endl;
    cout << "Alignment 2:     " << result.alignment2 << endl;

    return 0;
}


