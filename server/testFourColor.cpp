//
// Created by Qiu Yuqing on 2023/4/11.
//
#include <iostream>
#include <string.h>

#include "src/fourColor.h"
#include "src/testGenerator.h"

using namespace std;

// int main(int argc, char *argv[]) {
//    // ./fourcolor [n] [e] [scale] [edgeTimeOut] [solveTimeout] [inputFileName]
//    std::vector<std::string> return_status_array = {"Success", "Timeout", "Failure", "Wrong"};
//
//    if (argc != 7) {
//        cout << "ERROR: argument number should be 6\n";
//    }
//    int n = atoi(argv[1]);
//    int e = atoi(argv[2]);
//    auto scale = (float)atoi(argv[3]);
//    int edgeTimeOut = atoi(argv[4]);
//    int solveTimeOut = atoi(argv[5]);
//    string inputFilePath(argv[6]);
//    string outputFilePath = inputFilePath.substr(0, inputFilePath.size() - 4) + "_output.txt";
//
//    // generate test case
//    testGenerator gen(n, e, scale, edgeTimeOut, inputFilePath);
//    if (!gen.generate()) {
//        return -1;
//    }
//
//    // solve graph
//    fourColorSolver solver(solveTimeOut);
//    cout << "Load File: " << return_status_array[solver.loadFromFile(inputFilePath)] << "\n";
//
//    auto start = std::chrono::high_resolution_clock::now();
//    auto result = return_status_array[solver.solveGraph()];
//    auto end = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
//
//    cout << "Solve Graph: " << result << "\n";
//    cout << "Time Cost: " << duration << " s\n";
//
//    solver.saveToFile(outputFilePath);
//    if (result == "Success") {
//        return 0;
//    } else {
//        return -2;
//    }
// }

int main(int argc, char *argv[]) {
    std::vector<std::string> return_status_array = {"Success", "Timeout", "Failure", "Wrong"};
    // ./fourColor testcase
    if (argc != 3) {
        cout << "ERROR: argument number should be 3: ./fourcolor testcaseFilePath sequentialOrNot(\"true\" or \"false\")\n";
        exit(-1);
    }
    string inputFilePath(argv[1]);
    char *seqOrNot = argv[2];
    bool seq = false;
    if (strcmp(seqOrNot, "true") == 0)
        seq = true;
    else if (strcmp(seqOrNot, "false") != 0) {
        cout << "ERROR: 2nd argument should be either \"true\" or \"false\"!" << endl;
        exit(-1);
    }

    string outputFilePath = inputFilePath.substr(0, inputFilePath.size() - 4) + "_output_seq.txt";
    if (!seq) {
        outputFilePath = inputFilePath.substr(0, inputFilePath.size() - 4) + "_output_par.txt";
    }

    // solve graph
    int solveTimeOut = 15;
    fourColorSolver solver(solveTimeOut, seq);

    auto fileLoadResult = return_status_array[solver.loadFromFile(inputFilePath)];
    cout << "Load File: " << fileLoadResult << "\n";
    if (fileLoadResult != "Success") {
        return -1;
    }

    auto start = std::chrono::high_resolution_clock::now();
    auto result = return_status_array[solver.solveGraph()];
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    cout << "Solve Graph: " << result << "\n";
    cout << "Time Cost: " << duration << " ms\n";

    solver.saveToFile(outputFilePath);
    return 0;
}