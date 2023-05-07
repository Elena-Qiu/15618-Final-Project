#include <fstream>
#include <sstream>
#include <cstdlib>
#include <numeric>
#include <algorithm>
#include "fourColor.h"
#include <omp.h>

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
        // skip empty line
        if (line.empty()) {
            continue;
        }

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

void fourColorSolver::saveNodeAdjListToFile(std::string &fileName) {
    std::ofstream outFile(fileName);
    if (!outFile) {
        std::cout << "error writing file \"" << fileName << "\"" << std::endl;
        return;
    }
    outFile << nodeNum << std::endl;
    for (int i = 0; i < nodeNum; i++) {
        outFile << i << ": [";
        for (auto v : adjacentLists[i]) {
            outFile << v << " ";
        }
        outFile << "]\n";
    }
    outFile.close();
    if (!outFile)
        std::cout << "error writing file \"" << fileName << "\"" << std::endl;
}

void fourColorSolver::setGraph(int n, const std::vector<std::pair<int, int>> &edges) {
    nodeNum = n;
    colors.resize(nodeNum, -1);
    adjacentLists.resize(nodeNum, std::vector<int>());
    for (auto &e : edges) {
        int u = e.first;
        int v = e.second;
        adjacentLists[u].push_back(v);
        adjacentLists[v].push_back(u);
    }
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

int fourColorSolver::bruteForceHelperPar(int n, time_point start, const std::vector<int> &curColors) {
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
        #pragma omp task firstprivate(privateColors) shared(adjacentLists, rst) untied
        {
            #pragma omp cancellation point taskgroup
            if (rst != SUCCESS) {
                bool canColor = true;
                for (auto &v: adjacentLists[n]) {
//                     printf("[thread %d] checking node %d (color %d) with its neighbor %d (color %d)\n", omp_get_thread_num(), n, c, v, privateColors[v]);
                    if (privateColors[v] == c) {
                        canColor = false;
//                         printf("[thread %d] node %d cannot be colored with %d, so skip to next node\n", omp_get_thread_num(), n, c);
                        break;
                    }
                }
                if (canColor) {
                    privateColors[n] = c;
//                     printf("[thread %d] node %d is colored with %d\n", omp_get_thread_num(), n, c);
                    int nextRst = bruteForceHelperPar(n + 1, start, privateColors);
                    if (nextRst == SUCCESS) {
                        #pragma omp critical
                        {
                            rst = SUCCESS;
                        }
//                         printf("[thread %d] found solution!\n", omp_get_thread_num());
                        #pragma omp cancel taskgroup
                    } else {
//                         printf("[thread %d] further trying indicates node %d cannot be colored with %d, return back\n", omp_get_thread_num(), n, c);
                        privateColors[n] = -1;
                    }
                }
            }
        }
    }
    #pragma omp taskwait
    return rst;
}

int fourColorSolver::bruteForceHelperSeq(int n, time_point start) {
    // check timeout
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
    if (duration > timeOut) {
        return TIMEOUT;
    }

    // base case
    if (n == nodeNum) {
        return SUCCESS;
    }

    for (int c = 0; c < 4; c++) {
        bool canColor = true;
        for (auto & v: adjacentLists[n]) {
            if (colors[v] == c) {
                canColor = false;
                break;
            }
        }
        if (!canColor) {
            continue;
        }
        colors[n] = c;
        if (bruteForceHelperSeq(n + 1, start) == SUCCESS) {
            return SUCCESS;
        }
        colors[n] = -1;
    }
    return FAILURE;
}

int fourColorSolver::bruteForce() {
    if (putenv((char *) "OMP_CANCELLATION=true"))    {printf("set environment variable failed\n"); exit(-1);}
    // reinitialize colors to -1
    std::fill(colors.begin(), colors.end(), -1);
    auto start = std::chrono::high_resolution_clock::now();

    // sequential execution
    if (seq) {
        return bruteForceHelperSeq(0, start);
    }

    // parallel execution
    int rst;
    #pragma omp parallel shared(start)
    {
        auto colors_copy = colors;
        #pragma omp single
        {
            #pragma omp taskgroup
            {
                rst = SUCCESS;
                rst = bruteForceHelperPar(0, start, colors_copy);
            }
        }
    }
    return rst;
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
    auto rst = bruteForce();
    if (rst == SUCCESS && !checkSolution()) {
        rst = WRONG;
    }
    return rst;
}
