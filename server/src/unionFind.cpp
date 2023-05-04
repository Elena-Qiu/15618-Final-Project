//
// Created by Qiu Yuqing on 2023/5/3.
//
#include <iostream>
#include "unionFind.h"

unionFind::unionFind(const std::vector<std::vector<int> > &encodedId) {
    for (auto &v : encodedId) {
        for (auto &id : v) {
            // if (mapping.count(id)) {
            //     std::cout << "ERROR[UnionFind]: Duplicate encoded Id " << id << std::endl;
            // }
            mapping[id] = id;
            ranks[id] = 1;
        }
    }
}

void unionFind::unionPair(int x, int y) {
    // if (!mapping.count(x)) {
    //     std::cout << "ERROR[UnionFind]: Trying to union non-exist key " << x << "with " << y << std::endl;
    // }
    // if (!mapping.count(y)) {
    //     std::cout << "ERROR[UnionFind]: Trying to union non-exist key " << y << "with " << x << std::endl;
    // }
    int rootX = find(x);
    int rootY = find(y);
    if (rootX == rootY) {
        return;
    }
    if (ranks[rootX] < ranks[rootY]) {
        mapping[rootX] = rootY;
    } else if (ranks[rootX] > ranks[rootY]) {
        mapping[rootY] = rootX;
    } else {
        mapping[rootX] = rootY;
        ranks[rootY] += 1;
    }
}

int unionFind::find(int x) {
    if (!mapping.count(x)) {
        std::cout << "ERROR[UnionFind]: Trying to find non-exist key " << x << std::endl;
    }
    if (mapping[x] != x) {
        mapping[x] = find(mapping[x]);
    }
    return mapping[x];
}

std::unordered_map<int,int> unionFind::getCompressedMapping() {
    std::unordered_map<int,int> compressedId;
    std::unordered_map<int,int> compressedMapping;
    // get all roots
    for (auto &p : mapping) {
        int root = find(p.first);
        if (!compressedId.count(root)) {
            compressedId.insert({root, 0});
        }
    }
    // assign compressed id to all roots
    compressedIdNum = compressedId.size();
    int curId = 0;
    for (auto &p : compressedId) {
        compressedId[p.first] = curId++;
    }
    // change mapping to map to compressed id
    for (auto &p : mapping) {
        int root = find(p.first);
        compressedMapping[p.first] = compressedId[root];
    }
    return compressedMapping;
}
