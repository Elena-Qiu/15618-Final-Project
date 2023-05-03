//
// Created by Qiu Yuqing on 2023/5/3.
//

#include "unionFind.h"

unionFind::unionFind(const std::vector<std::vector<int>> &encodedId) {
    for (auto &v : encodedId) {
        for (auto &id : v) {
            mapping[id] = id;
            ranks[id] = 1;
        }
    }
}

void unionFind::unionPair(int x, int y) {
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
    if (mapping[x] != x) {
        mapping[x] = find(mapping[x]);
    }
    return mapping[x];
}

void unionFind::compressMapping() {
    std::unordered_map<int,int> compressedId;
    // get all roots
    for (auto &p : mapping) {
        int root = find(p.first);
        if (!compressedId.count(root)) {
            compressedId[root] = 0;
        }
    }
    // assign compressed id to all roots
    int curId = 0;
    for (auto &p : compressedId) {
        compressedId[p.first] = curId++;
    }
    // change mapping to map to compressed id
    for (auto &p : mapping) {
        int root = find(p.first);
        mapping[p.first] = compressedId[root];
    }
}
