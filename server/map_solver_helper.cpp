#include "map_solver.h"

/**
 * map solver
 * @param w[in] the width of the image
 * @param h[in] the height of the image
 * @param input[in] the array of nodes map (length: w * h)
 * @param output[out] the array of nodes map (length: w * h)
                    (memory for this array already allocated, so no need to malloc)
 *
 * @return true if solving succeeds, false otherwise
 */
bool solveMapHelper(int w, int h, const std::vector<int> &input, std::vector<int> &output) {
    // just return the original nodes map
    output = input;
    return true;
}