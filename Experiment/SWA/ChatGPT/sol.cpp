#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;

// Scoring system: match, mismatch, and gap penalties
const int MATCH_SCORE = 2;
const int MISMATCH_PENALTY = -1;
const int GAP_PENALTY = -1;

// Function to calculate the Smith-Waterman alignment
pair<int, pair<string, string>> smith_waterman(const string &seq1, const string &seq2) {
	int m = seq1.length();
	int n = seq2.length();

	// Initialize scoring matrix and traceback matrix
	vector<vector<int>> score(m + 1, vector<int>(n + 1, 0));
	vector<vector<pair<int, int>>> traceback(m + 1, vector<pair<int, int>>(n + 1, {-1, -1}));

	int max_score = 0;
	pair<int, int> max_pos = {0, 0}; // Track the position of the max score

	// Fill the scoring matrix
	for (int i = 1; i <= m; ++i) {
    	for (int j = 1; j <= n; ++j) {
        	int match = (seq1[i - 1] == seq2[j - 1]) ? MATCH_SCORE : MISMATCH_PENALTY;
        	int score_diag = score[i - 1][j - 1] + match;
        	int score_up = score[i - 1][j] + GAP_PENALTY;
        	int score_left = score[i][j - 1] + GAP_PENALTY;

        	// Take the maximum of these four options (including zero for local alignment)
        	score[i][j] = max({0, score_diag, score_up, score_left});

        	// Update the traceback matrix
        	if (score[i][j] == score_diag) {
            	traceback[i][j] = {i - 1, j - 1}; // Diagonal
        	} else if (score[i][j] == score_up) {
            	traceback[i][j] = {i - 1, j}; // Up
        	} else if (score[i][j] == score_left) {
            	traceback[i][j] = {i, j - 1}; // Left
        	}

        	// Track the maximum score position for traceback
        	if (score[i][j] > max_score) {
            	max_score = score[i][j];
            	max_pos = {i, j};
        	}
    	}
	}

	// Traceback to get the optimal local alignment
	string aligned_seq1 = "", aligned_seq2 = "";
	int i = max_pos.first, j = max_pos.second;

	while (i > 0 && j > 0 && score[i][j] != 0) {
    	pair<int, int> prev = traceback[i][j];
    	if (prev == make_pair(i - 1, j - 1)) {
        	aligned_seq1 = seq1[i - 1] + aligned_seq1;
        	aligned_seq2 = seq2[j - 1] + aligned_seq2;
    	} else if (prev == make_pair(i - 1, j)) {
        	aligned_seq1 = seq1[i - 1] + aligned_seq1;
        	aligned_seq2 = '-' + aligned_seq2;
    	} else if (prev == make_pair(i, j - 1)) {
        	aligned_seq1 = '-' + aligned_seq1;
        	aligned_seq2 = seq2[j - 1] + aligned_seq2;
    	}
    	i = prev.first;
    	j = prev.second;
	}

	// Return the max score and the aligned sequences
	return {max_score, {aligned_seq1, aligned_seq2}};
}

// Function to read sequence from a file
string read_sequence_from_file(const string &filename) {
	ifstream file(filename);
	string sequence;

	if (!file) {
    	cerr << "Error opening file: " << filename << endl;
    	exit(1);
	}

	getline(file, sequence); // Assumes the sequence is on one line
	file.close();
	return sequence;
}

int main() {
	// Read sequences from files
	string seq1 = read_sequence_from_file("./Experiment/SWA/seq1.txt");
	string seq2 = read_sequence_from_file("./Experiment/SWA/seq2.txt");

	auto result = smith_waterman(seq1, seq2);

	// Store results (you can process them further as needed)
	int max_score = result.first;
	string aligned_seq1 = result.second.first;
	string aligned_seq2 = result.second.second;

	return 0;
}
