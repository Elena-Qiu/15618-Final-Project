//
// Created by Qiu Yuqing on 2023/4/28.
//
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <math.h>
#include <algorithm>
#include <iostream>
#include "conversion.h"

int Conversion::getPixel(int x, int y) {
    return pixelToNode[y * w + x];
}

void Conversion::setPixel(int x, int y, int id) {
    pixelToNode[y * w + x] = id;
}

void Conversion::printPixelToNodes() {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (getPixel(x, y) != -2) {
                std::cout << getPixel(x, y) << " ";
            } else {
                std::cout << "| ";
            }
        }
        std::cout << std::endl;
    }
}

int Conversion::loadFromFile(std::string &fileName) {
    std::ifstream inFile;
    inFile.open(fileName);
    if (!inFile) {
        return FAILURE;
    }
    std::string line;

    // read w and h and initialize pixelToNode to w * h
    std::getline(inFile, line);
    std::stringstream sstream_w(line);
    std::string str;
    std::getline(sstream_w, str, '\n');
    w = (int)atoi(str.c_str());
    std::getline(inFile, line);
    std::stringstream sstream_h(line);
    std::getline(sstream_h, str, '\n');
    h = (int)atoi(str.c_str());

    // read nodes map
    while (std::getline(inFile, line)) {
        // skip empty line
        if (line.empty()) {
            continue;
        }

        std::stringstream sstream(line);
        std::string str;
        std::getline(sstream, str, '\n');
        pixelToNode.push_back((int)atoi(str.c_str()));
    }
    // TODO: delete this
    setPixel(w - 1, h - 1, -2);
    inFile.close();
    return SUCCESS;
}

void Conversion::saveNodesMapToFile(std::string &fileName) {
    std::ofstream outFile(fileName);
    if (!outFile) {
        std::cout << "error writing file \"" << fileName << "\"" << std::endl;
        return;
    }
    outFile << w << std::endl;
    outFile << h << std::endl;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            outFile << getPixel(x, y) << " ";
        }
        outFile << std::endl;
    }
    outFile.close();
    if (!outFile)
        std::cout << "error writing file \"" << fileName << "\"" << std::endl;
}

void Conversion::saveToFile(std::string &fileName) {
    std::ofstream outFile(fileName);
    if (!outFile) {
        std::cout << "error writing file \"" << fileName << "\"" << std::endl;
        return;
    }
    outFile << nodeNum << std::endl;
    for (int i = 0; i < nodeNum; i++) {
        outFile << std::endl;
    }
    for (auto &e : edges) {
        outFile << e.first << " " << e.second << std::endl;
    }
    outFile.close();
    if (!outFile)
        std::cout << "error writing file \"" << fileName << "\"" << std::endl;
}

void Conversion::findNodes() {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (getPixel(x, y) == -1) {
                auto localMarginalPoints = fillArea(x, y, nodeNum);
                if (!localMarginalPoints.empty()) {
                    nodeNum++;
                    marginalPoints.push_back(localMarginalPoints);
                }
            }
        }
    }
}

std::vector<Point> Conversion::fillArea(int x, int y, int id) {
    int n = 0;
    std::vector<std::vector<bool>> visited(w, std::vector<bool>(h, false));
    std::vector<Point> localMarginalPoints;
    std::queue<Point> qu;
    qu.emplace(Point{x, y});
    std::vector<std::pair<int,int>> dirs = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    while (!qu.empty()) {
        // pop point from list and color it
        Point p = qu.front();
        qu.pop();
        bool is_marginal = false;
        setPixel(p.x, p.y, id);
        n += 1;

        // check neighbors
        for (auto &dir : dirs) {
            int nx = p.x + dir.first;
            int ny = p.y + dir.second;
            // skip if visited
            if (visited[nx][ny]) {
                continue;
            }
            if (getPixel(nx, ny) == -1)
                qu.push({nx, ny});
            else if (getPixel(nx, ny) == -2)
                is_marginal = true;
            visited[nx][ny] = true;
        }

        // check if marginal
        if (is_marginal)
            localMarginalPoints.push_back(p);
    }
    // one-pixel bug: if only one pixel, don't count it as separate area
    if (n == 1) {
        setPixel(x, y, -2);
        localMarginalPoints.clear();
    }
   return localMarginalPoints;
}


double getDistance(int x1, int y1, int x2, int y2) {
    return sqrt((double)(x1 - x2) * (double)(x1 - x2) + (double)(y1 - y2) * (double)(y1 - y2));
}

void Conversion::findEdges() {
    std::vector<std::unordered_set<int>> adjacentLists(nodeNum);
    std::vector<std::unordered_map<int,int>> tmpEdges(nodeNum);
    // compare and check if nodes have an edge
    for (int i = 0; i < nodeNum; i++) {
        std::unordered_set<int> visited_neighbors; // neighbors that have been visited
        std::vector<Point>& mp = marginalPoints[i];
        for (Point &p : mp) {
            // check the surrounding points of p to see if they belong to other nodes
            for (int k = -MAX_LINE_THICKNESS; k < MAX_LINE_THICKNESS; k++) {
                for (int l = -MAX_LINE_THICKNESS; l < MAX_LINE_THICKNESS; l++) {
                    int tmpx = p.x + k;
                    int tmpy = p.y + l;

                    // if out of boundary, skip
                    if (tmpx < 0 || tmpx >= w || tmpy < 0 || tmpy >= h) {
                        continue;
                    }

                    // if out of distance, skip
                    if (getDistance(tmpx, tmpy, p.x, p.y) >= (double)MAX_LINE_THICKNESS)
                        continue;
                    int global_idx = tmpy * w + tmpx;

                    // if already visited, skip
                    if (visited_neighbors.count(global_idx))
                        continue;
                    visited_neighbors.insert(global_idx);

                    int tmpId = getPixel(tmpx, tmpy);
                    if (tmpId >= 0 && tmpId != i) {
                        // if already added as an edge, skip
                        if (adjacentLists[i].count(tmpId))
                            continue;
                        // if already added as a temporary edge, increment the counter
                        if (tmpEdges[i].count(tmpId)) {
                            tmpEdges[i][tmpId] += 1;
                            if (tmpEdges[i][tmpId] > EDGE_THRESHOLD) {
                                adjacentLists[i].insert(tmpId);
                                adjacentLists[tmpId].insert(i);
                            }
                        } else {
                            // add the edge to temp edges array
                            tmpEdges[i].insert({tmpId, 1});
                            tmpEdges[tmpId].insert({i, 1});
                        }
                    }
                }
            }
        }
    }

    // convert adjacentLists to Edges
    for (int i = 0; i < nodeNum; i++) {
        auto tmp = adjacentLists[i];
        std::vector<int> neighbors;
        neighbors.assign(tmp.begin(), tmp.end());
        std::sort(neighbors.begin(), neighbors.end());
        for (auto &j : neighbors) {
            if (j > i) {
                edges.emplace_back(i, j);
            }
        }
    }
}

void Conversion::convertMapToGraph() {
    findNodes();
    findEdges();
}

void Conversion::addMapColors(const std::vector<int>& colors) {
    // update pixelToNode with colors
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int id = getPixel(x, y);
            if (id >= 0) {
                setPixel(x, y, colors[id]);
            }
        }
    }
}

bool Conversion::checkNodesMap() {
    bool rst = true;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int id = getPixel(x, y);
            if (id == -1) {
                std::cout << "pixel[" << x << "][" << y << "] is -1\n";
                rst = false;
            }
        }
    }
    return rst;
}
