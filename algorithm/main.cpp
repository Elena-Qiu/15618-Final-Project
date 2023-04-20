//
// Created by Qiu Yuqing on 2023/4/11.
//
#include <iostream>
#include "fourColor.h"
#include "testGenerater.h"

using namespace std;

int main(int argc, char *argv[]) {
    // ./FourColor [n] [e] [scale] [edgeTimeOut] [solveTimeout] [inputFileName]
    if (argc != 7) {
        cout << "ERROR: argument number should be 6\n";
    }
    int n = atoi(argv[1]);
    int e = atoi(argv[2]);
    auto scale = (float)atoi(argv[3]);
    int edgeTimeOut = atoi(argv[4]);
    int solveTimeOut = atoi(argv[5]);
    string inputFileName(argv[6]);
    string outputFileName = inputFileName + "_output.txt";
    inputFileName += ".txt";

    // generate test case
    testGenerator gen(n, e, scale, edgeTimeOut, inputFileName);
    if (!gen.generate()) {
        return -1;
    }

    // solve graph
    fourColorSolver solver(solveTimeOut);
    cout << "Load File: " << solver.loadFromFile(inputFileName) << "\n";

    auto start = std::chrono::high_resolution_clock::now();
    auto result = solver.solveGraph();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();

    cout << "Solve Graph: " << result << "\n";
    cout << "Time Cost: " << duration << " s\n";

    solver.saveToFile(outputFileName);
    if (result == "Heuristic Success" || result == "BruteForce Success") {
        return 0;
    } else {
        return -2;
    }
}