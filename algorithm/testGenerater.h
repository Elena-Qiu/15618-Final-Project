//
// Created by Qiu Yuqing on 2023/4/16.
//

#ifndef FOURCOLOR_TESTGENERATER_H
#define FOURCOLOR_TESTGENERATER_H

#include <set>
#include <unordered_set>
#include <vector>

struct Pos{
    float x;
    float y;
    bool operator==(const Pos& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }
};

struct PosHashFunction {
    size_t operator()(const Pos& p) const
    {
        return (std::hash<float>()(p.x)) ^ (std::hash<float>()(p.y));
    }
};

struct Edge{
    int u;
    int v;
    bool operator<(const Edge& rhs) const
    {
        return u == rhs.u ? v < rhs.v : u < rhs.u;
    }
    bool operator==(const Edge& rhs) const
    {
        return u == rhs.u && v == rhs.v;
    }
};


class testGenerator {
    int nodeNum = 0;
    int edgeNum = 0;
    float scale = 100.0;
    int timeout = 10;
    std::string outfileName;
    std::vector<Pos> nodes;
    std::set<Edge> edges;

    float getRand();
    bool checkEdge(Edge e1);
    void generateNodes();
    bool generateEdges();
    void outputToFile();

public:
    testGenerator(int n, int e, float s, int t, std::string f);
    bool generate();
};


#endif //FOURCOLOR_TESTGENERATER_H
