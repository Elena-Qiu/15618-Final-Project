//
// Created by Qiu Yuqing on 2023/4/30.
//
#include <iostream>
#include "conversion.h"

using namespace std;

int main(int argc, char *argv[]) {
    // ./conversion-release testcase
    if (argc != 2) {
        cout << "ERROR: argument number should be 2: ./fourcolor-release testcaseFilePath\n";
        exit(-1);
    }

    string inputFilePath(argv[1]);
    string outputFilePath = inputFilePath.substr(0, inputFilePath.size() - 11) + "_output.txt";

    // solve graph
    int solveTimeOut = 60;
    Conversion solver(solveTimeOut);
    solver.testMode = true;

    auto fileLoadResult = return_status_array[solver.loadFromFile(inputFilePath)];
    cout << "Load File: " << fileLoadResult << "\n";
    if (fileLoadResult != "Success") {
        return -1;
    }

    auto start = std::chrono::high_resolution_clock::now();
    auto result = return_status_array[solver.solveMap()];
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    cout << "Solve Map: " << result << "\n";
    cout << "Time Cost: " << duration << " ms\n";

//    solver.printPixelToNodes();

    solver.saveToFile(outputFilePath);
    return 0;
}