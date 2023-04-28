#include <fstream>
#include <sstream>
#include <cstdlib>
#include <numeric>
#include <algorithm>
#include "fourColor.h"

int fourColorSolver::loadFromFile(std::string &fileName) {
    std::ifstream inFile;
    inFile.open(fileName);
    if (!inFile) {
        return FAILURE;
    }
    std::string line;

    // read the number of nodes, initialize colors to -1
    std::getline(inFile, line);
    std::stringstream sstream(line);
    std::string str;
    std::getline(sstream, str, '\n');
    nodeNum = (int)atoi(str.c_str());
    colors.resize(nodeNum, -1);
    adjacentLists.resize(nodeNum, std::vector<int>());

    // skip node positions
    for (int i = 0; i < nodeNum; i++) {
        std::getline(inFile, line);
    }

    // read edges
    while (std::getline(inFile, line)) {
        int u, v;
        std::stringstream sstream(line);
        std::string str;
        std::getline(sstream, str, ' ');
        u = (int)atoi(str.c_str());
        std::getline(sstream, str, '\n');
        v = (int)atoi(str.c_str());
        adjacentLists[u].push_back(v);
        adjacentLists[v].push_back(u);
    }
    inFile.close();
    return SUCCESS;
}

void fourColorSolver::saveToFile(std::string &fileName) {
    std::ofstream outFile(fileName);
    if (!outFile) {
        std::cout << "error writing file \"" << fileName << "\"" << std::endl;
        return;
    }
    for (auto c : colors) {
        outFile << c << std::endl;
    }
    outFile.close();
    if (!outFile)
        std::cout << "error writing file \"" << fileName << "\"" << std::endl;
}

bool fourColorSolver::heuristic() {
    // find valence for nodes
    std::vector<size_t> valence(nodeNum, 0);
    for (int i = 0; i < nodeNum; i++) {
        valence[i] = adjacentLists[i].size();
    }

    // get indices of nodes sorted on valence
    std::vector<int> sortedIndices(nodeNum, 0);
    std::iota(sortedIndices.begin(), sortedIndices.end(), 0);
    stable_sort(sortedIndices.begin(), sortedIndices.end(),
                [&valence](size_t i1, size_t i2){
                            return valence[i1] > valence[i2];
                       }
    );

    // begin coloring using Welsh-Powell algorithm
    int numColored = 0;
    bool success = false;
    for (int c = 0; c < 4; c++) {
        for (auto & u : sortedIndices) {
            if (colors[u] == -1) {
                bool canColor = true;
                for (auto & v: adjacentLists[u]) {
                    if (colors[v] == c) {
                        canColor = false;
                        break;
                    }
                }
                if (canColor) {
                    colors[u] = c;
                    numColored++;
                    if (numColored == nodeNum) {
                        success = true;
                        break;
                    }
                }
            }
        }
        if (success) {
            break;
        }
    }
    return success;
}

int fourColorSolver::bruteForceHelper(int n, time_point start, std::vector<int> &curColors) {
    // check timeout
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
    if (duration > timeOut) {
        return TIMEOUT;
    }

    // base case
    if (n == nodeNum) {
        colors = curColors;
        return SUCCESS;
    }

    // recursion
    int rst = FAILURE;
    for (int c = 0; c < 4; c++) {
        auto privateColors = curColors;
        #pragma omp taskgroup
        {
            #pragma omp task firstprivate(privateColors) shared(adjacentLists, rst) untied
            {
                bool canColor = true;
                for (auto &v: adjacentLists[n]) {
                    if (privateColors[v] == c) {
                        canColor = false;
                        break;
                    }
                }
                #pragma omp cancellation point taskgroup
                if (canColor) {
                    privateColors[n] = c;
                    int nextRst = bruteForceHelper(n + 1, start, privateColors);
                    #pragma omp critical
                    {
                        rst = nextRst;
                    }
                    if (nextRst == SUCCESS) {
                        #pragma omp cancel taskgroup
                    }
                    privateColors[n] = -1;
                }
            }
        }
    }
    #pragma omp taskwait
    return rst;


//    for (int c = 0; c < 4; c++) {
//        bool canColor = true;
//        for (auto & v: adjacentLists[n]) {
//            if (colors[v] == c) {
//                canColor = false;
//                break;
//            }
//        }
//        if (!canColor) {
//            continue;
//        }
//        colors[n] = c;
//        rst = bruteForceHelper(n + 1, start, colors);
//        if (rst == SUCCESS) {
//            break;
//        }
//        colors[n] = -1;
//    }
//    return rst;

}

int fourColorSolver::bruteForce() {
    // reinitialize colors to -1
    std::fill(colors.begin(), colors.end(), -1);
    auto start = std::chrono::high_resolution_clock::now();
    int rst;
    #pragma omp parallel shared(start)
    {
        #pragma omp single
        {
            rst = bruteForceHelper(0, start, colors);
        }
    }
    return rst;
//    return bruteForceHelper(0, start);
}

bool fourColorSolver::checkSolution() {
    for (int i = 0; i < nodeNum; i++) {
        int color = colors[i];
        for (auto v : adjacentLists[i]) {
            if (color == colors[v]) {
                return false;
            }
        }
    }
    return true;
}

int fourColorSolver::solveGraph() {
//    if (heuristic()) {
//        if (!checkSolution()) {
//            return "Heuristic Failure";
//        }
//        return "Heuristic Success";
//    } else if (bruteForce()) {
//        if (!checkSolution()) {
//            return "BruteForce Failure";
//        }
//        return "BruteForce Success";
//    }
    auto rst = bruteForce();
    if (!checkSolution()) {
        rst = WRONG;
    }
    return rst;
}