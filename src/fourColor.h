//
// Created by Qiu Yuqing on 2023/4/11.
//

#ifndef FOURCOLOR_FOURCOLOR_H
#define FOURCOLOR_FOURCOLOR_H

#include <vector>
#include <iostream>
#include <chrono>

using time_point = std::chrono::high_resolution_clock::time_point;


class fourColorSolver {
public:
    // constructor
    explicit fourColorSolver(int t = 10, bool _seq = true) {
        seq = _seq;
        timeOut = t;
        nodeNum = 0;
    }

    // file input and output
    int loadFromFile(std::string &fileName);
    void saveToFile(std::string &fileName);

    // API input and output
    void setGraph(int n, const std::vector<std::pair<int,int>>& edges);
    std::vector<int>& getColors() {
        return colors;
    }

    // algorithm
    int solveGraph();

    // for debugging
    void saveNodeAdjListToFile(std::string &fileName);

private:
    bool seq;
    int timeOut;
    int nodeNum;
    std::vector<std::vector<int>> adjacentLists;
    std::vector<int> colors;

    bool heuristic();
    int bruteForce();
    int bruteForceHelperSeq(int n, time_point start);
    int bruteForceHelperPar(int n, time_point start, const std::vector<int> &curColors);
    bool checkSolution();

    // fix this
    enum return_status {SUCCESS, TIMEOUT, FAILURE, WRONG};
    std::vector<std::string> return_status_array = {"Success", "Timeout", "Failure", "Wrong"};
};

#endif //FOURCOLOR_FOURCOLOR_H
