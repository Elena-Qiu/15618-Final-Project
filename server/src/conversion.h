//
// Created by Qiu Yuqing on 2023/4/28.
//

#include <vector>
#include <iostream>
#include <chrono>

#ifndef INC_15618_FINAL_PROJECT_CONVERSION_H
#define INC_15618_FINAL_PROJECT_CONVERSION_H

using time_point = std::chrono::high_resolution_clock::time_point;

const int LINE_EXPANSION = 0;
const int MAX_LINE_THICKNESS = 2 * LINE_EXPANSION + 3;
const int EDGE_THRESHOLD = 3;

typedef struct Point {
    int x;
    int y;
}point_t;

class Conversion {
public:

    // constructor
    explicit Conversion(int timeOut = 10) {
        w = 0;
        h = 0;
        nodeNum = 0;
    }

    // file input and output
    int loadFromFile(std::string &fileName);
    void saveToFile(std::string &fileName);

    // API input and output
    void setPixelToNodeArray(int _w, int _h, const std::vector<int>& arr) {
        w = _w;
        h = _h;
        pixelToNode = arr;
    }
    std::vector<int>& getPixelToNodeArray() {
        return pixelToNode;
    }

    int getNodeNum() const {
        return nodeNum;
    }

    std::vector<std::pair<int,int>> getEdges() {
        return edges;
    }

    // functions
    void convertMapToGraph();
    void addMapColors(const std::vector<int>& colors);

    // debug
    void printPixelToNodes();
    void saveNodesMapToFile(std::string &fileName);

    // TODO: fix this
    void findNodes(bool bfs=true);
    void findEdges();

private:
    int w;
    int h;
    int nodeNum;
    std::vector<int> pixelToNode;
    std::vector<std::vector<Point>> marginalPoints;
    std::vector<std::pair<int,int>> edges;

    int getPixel(int x, int y);
    void setPixel(int x, int y, int id);


    std::vector<Point> fillArea(int x, int y, int id, bool bfs);

    enum return_status {SUCCESS, TIMEOUT, FAILURE, WRONG};
    std::vector<std::string> return_status_array = {"Success", "Timeout", "Failure", "Wrong"};
};


#endif //INC_15618_FINAL_PROJECT_CONVERSION_H