//
// Created by Qiu Yuqing on 2023/4/16.
//

#include <iostream>
#include <utility>
#include <stdlib.h>
#include <fstream>
#include <unordered_set>
#include "testGenerater.h"

using namespace std;

testGenerator::testGenerator(int n, int e, float s, int t, string f) {
    nodeNum = n;
    edgeNum = e;
    scale = s;
    timeout = t;
    outfileName = std::move(f);
    if (e > n * (n - 1) / 2) {
        cout << "ERROR: Edges " << edgeNum << " is too much for node " << nodeNum << "\n";
    }
}

float testGenerator::getRand() {
    return -scale + (((float) rand()) / (float) RAND_MAX) * (2 * scale);
}

void testGenerator::generateNodes() {
    srand(time(NULL));
    unordered_set<Pos, PosHashFunction> positions;
    for (int i = 0; i < nodeNum; i++) {
        Pos p = {getRand(), getRand()};
        while (positions.count(p)) {
            p.x = getRand();
            p.y = getRand();
        }
        positions.insert(p);
        nodes.push_back(p);
    }
}

// --------------------------------------------------------------------------------
// ref: https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
bool onSegment(Pos p, Pos q, Pos r)
{
    if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
        q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
        return true;
    return false;
}

int orientation(Pos p, Pos q, Pos r)
{
    float val = (q.y - p.y) * (r.x - q.x) -
              (q.x - p.x) * (r.y - q.y);

    if (val == 0) return 0;  // collinear

    return (val > 0)? 1: 2; // clock or counterclock wise
}

bool doIntersect(Pos p1, Pos q1, Pos p2, Pos q2)
{
    // Find the four orientations needed for general and
    // special cases
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    // General case
    if (o1 != o2 && o3 != o4)
        return true;

    // Special Cases
    // p1, q1 and p2 are collinear and p2 lies on segment p1q1
    if (o1 == 0 && onSegment(p1, p2, q1)) return true;

    // p1, q1 and q2 are collinear and q2 lies on segment p1q1
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;

    // p2, q2 and p1 are collinear and p1 lies on segment p2q2
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;

    // p2, q2 and q1 are collinear and q1 lies on segment p2q2
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;

    return false; // Doesn't fall in any of the above cases
}
// --------------------------------------------------------------------------------

bool testGenerator::checkEdge(Edge e1) {
    for (auto & e2 : edges) {
        Pos p1 = nodes[e1.u];
        Pos q1 = nodes[e1.v];
        Pos p2 = nodes[e2.u];
        Pos q2 = nodes[e2.v];
        // two edges from the same node
        if (p1 == p2 || p1 == q2 || q1 == p2 || q1 == q2) {
            continue;
        }
        // check if intersect
        if (doIntersect(p1, q1, p2, q2)) {
            return false;
        }
    }
    return true;
}

bool testGenerator::generateEdges() {
    srand(time(NULL));
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < edgeNum; i++) {
        Edge e{};
        int r1 = rand() % (nodeNum - 1);
        int r2 = rand() % (nodeNum - 1);
        e.u = min(r1, r2);
        e.v = max(r1, r2);
        while (e.u == e.v || edges.count(e) || !checkEdge(e)) {
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
            if (duration > timeout) {
                return false;
            }
            r1 = rand() % (nodeNum - 1);
            r2 = rand() % (nodeNum - 1);
            e.u = min(r1, r2);
            e.v = max(r1, r2);
        }
        edges.insert(e);
    }

    // checking
    for (auto e : edges) {
        if (e.u < 0 || e.v < 0 || e.u >= nodeNum || e.v >= nodeNum) {
            cout << "ERROR: generate test case failure, node not within range\n";
            return false;
        }
        if (e.u == e.v) {
            cout << "ERROR: generate test case failure, cannot connect the same node\n";
            return false;
        }
        if (e.u > e.v) {
            cout << "ERROR: generate test case failure, first node larger than second one\n";
            return false;
        }
    }
    return true;
}

void testGenerator::outputToFile() {
    std::ofstream outFile(outfileName);
    if (!outFile) {
        std::cout << "error writing file \"" << outfileName << "\"" << std::endl;
        return;
    }
    outFile << nodeNum << std::endl;
    for (auto n : nodes) {
        outFile << n.x << " " << n.y << std::endl;
    }
    for (auto e : edges) {
        outFile << e.u << " " << e.v << std::endl;
    }
    outFile.close();
    if (!outFile)
        std::cout << "error writing file \"" << outfileName << "\"" << std::endl;
}

bool testGenerator::generate() {
    generateNodes();
    std::cout << "Finish generating nodes\n";
    if (!generateEdges()) {
        // timeout
        return false;
    }
    std::cout << "Finish generating edges\n";
    outputToFile();
    return true;
}