//
// Created by Qiu Yuqing on 2023/4/28.
//

#include <queue>
#include "conversion.h"

int conversion::getPixel(int x, int y) {
    return pixelToNode[y * w + x];
}

void conversion::setPixel(int x, int y, int id) {
    pixelToNode[y * w + x] = id;
}

int conversion::loadFromFile(std::string &fileName) {
    std::ifstream inFile;
    inFile.open(fileName);
    if (!inFile) {
        return FAILURE;
    }
    std::string line;

    // read w and h and initialize pixelToNode to w * h
    std::getline(inFile, line);
    std::stringstream sstream(line);
    std::string str;
    std::getline(sstream, str, '\n');
    w = (int)atoi(str.c_str());
    std::getline(sstream, str, '\n');
    h = (int)atoi(str.c_str());

    // read nodes map
    while (std::getline(inFile, line)) {
        int u, v;
        std::stringstream sstream(line);
        std::string str;
        std::getline(sstream, str, '\n');
        pixelToNode.push_back((int)atoi(str.c_str());)
    }
    inFile.close();
    return SUCCESS;
}

void conversion::saveToFile(std::string &fileName) {
    std::ofstream outFile(fileName);
    if (!outFile) {
        std::cout << "error writing file \"" << fileName << "\"" << std::endl;
        return;
    }
//    for (int y = 0; y < h; y++) {
//        for (int x = 0; x < w; x++) {
//            int p = getPixel(x, y);
//            outFile << p << " ";
//        }
//        outFile << std::endl;
//    }
    for (auto &p : pixelToNode) {
        outFile << p << std::endl;
    }
    outFile.close();
    if (!outFile)
        std::cout << "error writing file \"" << fileName << "\"" << std::endl;
}

void conversion::findNodes() {
//    for (let y = 0; y < h; y++) {
//        for (let x = 0; x < w; x++) {
//            if (get_nodes_map(x, y) === -1) {
//                let local_marginal_points = fill_area(x, y, nodes_num);
//                if (local_marginal_points.length > 0) {
//                    nodes_num ++;
//                    marginal_points.push(local_marginal_points);
//                }
//            }
//        }
//    }
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (getPixel(x, y) == -1) {

            }
        }
    }
}

void conversion::fillArea() {
    std::vector<int> localMarginalPoints;
    std::queue<int> qu;

}