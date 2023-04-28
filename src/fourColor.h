//
// Created by Qiu Yuqing on 2023/4/11.
//

#ifndef FOURCOLOR_FOURCOLOR_H
#define FOURCOLOR_FOURCOLOR_H

#include <vector>
#include <iostream>
#include <chrono>

using time_point = std::chrono::high_resolution_clock::time_point;

enum return_status {SUCCESS, TIMEOUT, FAILURE, WRONG};
static const std::vector<std::string> return_status_array = {"Success", "Timeout", "Failure", "Wrong"};

class fourColorSolver {
public:
    // constructor
    explicit fourColorSolver(int t) {
        timeOut = t;
    }

    // file input and output
    int loadFromFile(std::string &fileName);
    void saveToFile(std::string &fileName);

    // API input and output
    void setNodesEdges(int n, const std::vector<std::pair<int,int>>& edges);
    std::vector<int>& getColors() {
        return colors;
    }

    // algorithm
    int solveGraph();

private:
    int timeOut;
    int nodeNum;
    std::vector<std::vector<int>> adjacentLists;
    std::vector<int> colors;

    bool heuristic();
    int bruteForce();
    int bruteForceHelper(int n, time_point start, std::vector<int> &curColors);
    bool checkSolution();
};

#endif //FOURCOLOR_FOURCOLOR_H
