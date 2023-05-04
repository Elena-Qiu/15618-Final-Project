//
// Created by Qiu Yuqing on 2023/5/3.
//
#include <unordered_map>
#include <vector>

#ifndef SERVER_UNIONFIND_H
#define SERVER_UNIONFIND_H

// union find helper for parallelized findNodes in conversion.cpp
class unionFind {
public:
    explicit unionFind(const std::vector<std::vector<int>>& encodedId);

    void unionPair(int x, int y);
    std::unordered_map<int,int> getCompressedMapping();

private:
    std::unordered_map<int,int> mapping;
    std::unordered_map<int,int> ranks;

    int find(int x);
};


#endif //SERVER_UNIONFIND_H
