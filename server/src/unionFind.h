//
// Created by Qiu Yuqing on 2023/5/3.
//
#include <unordered_map>

#ifndef SERVER_UNIONFIND_H
#define SERVER_UNIONFIND_H


class unionFind {
public:
    unionFind(std::unordered_map<int,int> _mapping) {
        mapping = _mapping;
    }

private:
    std::unordered_map<int,int> mapping;



};


#endif //SERVER_UNIONFIND_H
