#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "src/unionFind.h"
using namespace std;

int main() {
    vector<vector<int>> encodedId = {{1,3,5}, {7,13,28}, {22, 40,35}};
    vector<pair<int,int>> conflict = {{1,3},{3, 5}, {7,13}, {7, 28}, {22,40}, {22, 35}, {35, 3}};
    unionFind uf(encodedId);
    for (auto c : conflict) {
        uf.unionPair(c.first, c.second);
    }
    auto mapping = uf.getCompressedMapping();
    for (auto p : mapping) {
        cout << p.first << ": " <<  p.second << "\n";
    }
}