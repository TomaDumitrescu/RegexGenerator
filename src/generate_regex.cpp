#include "generate_regex.hpp"

using namespace std;

// Calculates the greatest value of two numbers passed as parameters
int max(int a, int b) {
    return (a > b)? a : b;
}

int min(int a, int b) {
    return (a < b)? a : b;
}

// Analyzing different inputs, compute a probabilistic threshold for efficiency
int AI(int len) {
    return len / 5 + 1;
}

// Determine (I, J) to make the regex shorter and increase the probability of min disjoint pos intervals
int calculateMinPos(int lenStr, bool accLetters[][ALPHABET], bool failLetters[][ALPHABET], int *cnt) {
    int minCommon = INF, minPos = -1, countCommon = 0, threshold = AI(lenStr);
    for (int i = 0; i < lenStr; i++) {
        countCommon = 0;
        for (int j = 0; j < ALPHABET; j++) {
            if (accLetters[i][j] && failLetters[i][j]) {
                countCommon++;
            }
        }

        if (countCommon < minCommon) {
            minCommon = countCommon;
            minPos = i;
        } else if (countCommon == minCommon) {
            // It's useful to have the position somewhere in the middle
            if (minPos == -1 || (minPos <= threshold && i < lenStr - threshold)) {
                minPos = i;
            }
        }
    }

    *cnt = minCommon;

    return minPos;
}

string getRegex(int lenStr, bool accLetters[][ALPHABET], bool failLetters[][ALPHABET],
                int minPos, int minCommon) {
    string resultBuilder = "";
    if (minCommon == 0) {
        // The simplest cases
        for (int i = 0; i < lenStr; i++) {
            if (i == minPos) {
                resultBuilder += "(";
                char lastCommon = '$';
                for (int j = 0; j < ALPHABET; j++) {
                    if (accLetters[i][j]) {
                        lastCommon = j + 'a';
                    }
                }

                for (int j = 0; j < ALPHABET; j++) {
                    if (accLetters[i][j] && lastCommon != j + 'a') {
                        resultBuilder += (char)(j + 'a') + '|';
                    } else if (accLetters[i][j]) {
                        resultBuilder += (char)(j + 'a');
                    }
                }
                resultBuilder += ")";
            } else {
                resultBuilder += ".";
            }
        }
    } else {
        int start= minPos, end = minPos, acc, fail, eps_s = 1, eps_e = 1;
        // Chaining minimum intervals
        for (int neigh = 0; neigh < 2 * lenStr; neigh++) {
            if (neigh % 2 == 0) {
                eps_s++;
            } else {
                eps_e++;
            }

            start = max(0, minPos - eps_s);
            end = min(minPos + eps_e, lenStr - 1);

            ifstream in("input.txt");
            char c;
            in >> acc >> fail >> lenStr;

            vector<string> accChunks;
            unordered_map<string, bool> failChunks;
            for (int i = 0; i < acc; i++) {
                string word = "";
                for (int j = 0; j < lenStr; j++) {
                    in >> c;
                    if (j >= start && j <= end) {
                        word += c;
                    }
                }

                accChunks.emplace_back(word);
            }

            for (int i = 0; i < fail; i++) {
                string word = "";
                for (int j = 0; j < lenStr; j++) {
                    in >> c;
                    if (j >= start && j <= end) {
                        word += c;
                    }
                }

                failChunks[word] = true;
            }
            
            in.close();

            bool disjointSets = true;
            for (const auto& e: accChunks) {
                if (failChunks[e]) {
                    disjointSets = false;
                    break;
                }
            }

            if (disjointSets) {
                // Constructing the regex
                for (int i = 0; i <= start - 1; i++) {
                    resultBuilder += '.';
                }

                resultBuilder += '(';

                int cnt = 0;
                for (const auto& e: accChunks) {
                    cnt++;
                    if (failChunks[e]) {
                        continue;
                    }

                    if (cnt != accChunks.size()) {
                        resultBuilder += e + "|";
                    } else {
                        resultBuilder += e;
                    }

                    failChunks[e] = true;
                }

                resultBuilder += ')';

                resultBuilder += ".*";

                break;
            }
        }
    }

    return resultBuilder;
}

int main() {
    // Reading data from file input
    ifstream in(FILE_IN);
    int cntAccept = 0, cntFail = 0, lenStr = 0;
    bool accLetters[MAX_WORD_LEN][ALPHABET] = {0}, failLetters[MAX_WORD_LEN][ALPHABET] = {0};
    in >> cntAccept >> cntFail >> lenStr;

    char word[MAX_WORD_LEN] = {0};
    for (int i = 0; i < cntAccept; i++) {
        for (int j = 0; j < lenStr; j++) {
            in >> word[i];
            accLetters[j][word[i] - 'a'] = true;
        }
    }

    for (int i = 0; i < cntFail; i++) {
        for (int j = 0; j < lenStr; j++) {
            in >> word[i];
            failLetters[j][word[i] - 'a'] = true;
        }
    }

    int minCommon = 0;
    int minPos = calculateMinPos(lenStr, accLetters, failLetters, &minCommon);

    in.close();

    // Calculating the regex
    string result = getRegex(lenStr, accLetters, failLetters, minPos, minCommon);

    // Printing the result in the required format
    ofstream out(FILE_OUT);

    out << "regex\n";
    out << result.c_str();

    out.close();

    return 0;
}
