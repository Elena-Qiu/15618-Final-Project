//
// Created by Qiu Yuqing on 2023/4/28.
//

#include <vector>
#include <iostream>
#include <chrono>
#include <unordered_map>

#ifndef INC_15618_FINAL_PROJECT_CONVERSION_H
#define INC_15618_FINAL_PROJECT_CONVERSION_H

using time_point = std::chrono::high_resolution_clock::time_point;

const int LINE_EXPANSION = 0;
const int MAX_LINE_THICKNESS = 2 * LINE_EXPANSION + 3;
const int EDGE_THRESHOLD = 3;

typedef struct Point {
    int x;
    int y;
} point_t;

class Conversion {
public:

    // constructor
    explicit Conversion(bool _seq = true) {
        w = 0;
        h = 0;
        nodeNum = 0;
        seq = _seq;
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
    std::vector<int>& getPixelToNodeArray() {return pixelToNode;}
    int getNodeNum() const {return nodeNum;}
    std::vector<std::pair<int,int>> getEdges() {return edges;}

    // functions
    void convertMapToGraph();
    void addMapColors(const std::vector<int>& colors);

    // debug
    void printPixelToNodes();
    void saveNodesMapToFile(std::string &fileName);

    // TODO: fix this
    // for sequential
    void findNodesSeq(bool bfs=true);
    void findEdgesSeq();
    // for parallel
    void findNodesPar(bool bfs=true);
    void findEdgesPar();


private:
    int w;
    int h;
    int nodeNum;
    bool seq;
    std::vector<int> pixelToNode;
    std::vector<std::vector<Point>> marginalPoints;
    std::vector<std::pair<int,int>> edges;

    enum return_status {SUCCESS, TIMEOUT, FAILURE, WRONG};
    std::vector<std::string> return_status_array = {"Success", "Timeout", "Failure", "Wrong"};

    int getPixel(int x, int y);
    void setPixel(int x, int y, int id);

    std::vector<Point> fillAreaSeq(int x, int y, int id, bool bfs);

    // for parallel findNodes
    const int GRID_DIM = 8;
    std::vector<std::vector<int>> pixelToNodePar;
    std::unordered_map<int, int> nodeIdMapping; // map encoded node id to global node id

    void splitNodesMap();
    int getPixelPar(int gridIdxX, int gridIdxY, int localX, int localY);
    int getPixelPar(int globalX, int globalY);
    void setPixelPar(int gridIdxX, int gridIdxY, int localX, int localY, int id);
    void setPixelPar(int globalX, int globalY, int id);
    int getGridWidth(int gridIdxX);
    int getGridHeight(int gridIdxY);
    int encodeNodeId(int gridIdxX, int gridIdxY, int nodeId);
    int getGlobalX(int gridIdxX, int localX);  // x idx in 2d array
    int getGlobalY(int gridIdxY, int localY);  // y idx in 2d array
    int getGridIdxX(int globalX);
    int getGridIdxY(int globalY);
    int getLocalX(int localX);
    int getLocalY(int localY);

    std::vector<Point> fillAreaPar(int x, int y, int id, bool bfs);
    void findNodePairsForGrid(int threadId, std::unordered_set<std::pair<int, int>> &gridNodePairs, std::vector<std::vector<Point>> &gridMarginalPoints);
    void findNodesForGrid(bool bfs, int threadId, std::vector<std::vector<Point>> &gridMarginalPoints, std::vector<int> &gridEncodedNodeIds);
};


#endif //INC_15618_FINAL_PROJECT_CONVERSION_H