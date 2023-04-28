//
// Created by Qiu Yuqing on 2023/4/28.
//

#include <vector>
#include <iostream>
#include "fourColor.h"

#ifndef INC_15618_FINAL_PROJECT_CONVERSION_H
#define INC_15618_FINAL_PROJECT_CONVERSION_H


class conversion {
public:
    bool testMode = true;

    // constructor
    explicit conversion(int t) {
        timeOut = t;
    }

    // file input and output
    int loadFromFile(std::string &fileName);
    void saveToFile(std::string &fileName);

    // API input and output
    void setPixelToNodeArray(const std::vector<int>& arr) {
        pixelToNode = arr;
    }
    std::vector<int>& getPixelToNodeArray() {
        return pixelToNode;
    }

    // algorithm
    int solveMap();

private:
    int w;
    int h;
    int timeOut; // timeout to pass to the four color solver
    std::vector<int> pixelToNode;
    std::vector<std::vector<int>> marginalPoints;
    fourColorSolver graphSolver(timeOut);

    int getPixel(int x, int y);
    void setPixel(int x, int y, int id);

    void findNodes();
    void findEdges();
    void fillArea();
};


#endif //INC_15618_FINAL_PROJECT_CONVERSION_H
