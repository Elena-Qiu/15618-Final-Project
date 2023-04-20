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
    int nodeNum = 0;
    int timeOut = 10;
    std::vector<std::vector<int>> adjacentLists;
    std::vector<int> colors;

    explicit fourColorSolver(int t) {
        timeOut = t;
    }
    std::string loadFromFile(std::string &fileName);
    void saveToFile(std::string &fileName);
    std::string solveGraph();

private:
    bool heuristic();
    bool bruteForce();
    bool bruteForceHelper(int n, time_point start);
    bool checkSolution();
};

#endif //FOURCOLOR_FOURCOLOR_H
