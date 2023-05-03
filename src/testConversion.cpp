//
// Created by Qiu Yuqing on 2023/4/30.
//
#include <iostream>
#include "conversion.h"

using namespace std;

int main(int argc, char *argv[]) {
    std::vector<std::string> return_status_array = {"Success", "Timeout", "Failure", "Wrong"};

    // ./conversion-release testcase
    if (argc != 2) {
        cout << "ERROR: argument number should be 2: ./conversion-release testcaseFilePath\n";
        exit(-1);
    }

    string inputFilePath(argv[1]);
    string outputFilePath = inputFilePath.substr(0, inputFilePath.size() - 11) + "_output.txt";

    // solve graph
    Conversion converter;

    auto fileLoadResult = return_status_array[converter.loadFromFile(inputFilePath)];
    cout << "Load File: " << fileLoadResult << "\n";
    if (fileLoadResult != "Success") {
        return -1;
    }

    auto start = std::chrono::high_resolution_clock::now();
    converter.findNodes();
    auto end = std::chrono::high_resolution_clock::now();
    auto nodes_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    converter.findEdges();
    end = std::chrono::high_resolution_clock::now();
    auto edges_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    cout << "Find Nodes: " << nodes_duration << " ms\n";
    cout << "Find Edges: " << edges_duration << " ms\n";

    converter.saveToFile(outputFilePath);
    return 0;
}