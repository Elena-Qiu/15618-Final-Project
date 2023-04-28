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
    int nodeNum = 0;
    int timeOut = 10;
    std::vector<std::vector<int>> adjacentLists;
    std::vector<int> colors;

    explicit fourColorSolver(int t) {
        timeOut = t;
    }
    int loadFromFile(std::string &fileName);
    void saveToFile(std::string &fileName);
    int solveGraph();

private:
    bool heuristic();
    int bruteForce();
    int bruteForceHelper(int n, time_point start, std::vector<int> &curColors);
    bool checkSolution();
};

#endif //FOURCOLOR_FOURCOLOR_H
