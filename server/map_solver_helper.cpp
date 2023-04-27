#include "map_solver.h"

/**
 * map solver
 * @param w[in] the width of the image
 * @param h[in] the height of the image
 * @param input[in] the array of original pixels (length: w * h * 4, because each pixel is 
 *                  represented as four uint8 values for r, g, b, alpha, respectively)
 * @param output[out] the array of updated pixels (length: w * h * 4)
                    (memory for this array already allocated, so no need to malloc)
 *
 * @return true if solving succeeds, false otherwise
 */
bool solveMapHelper(int w, int h, const std::vector<char> &input, std::vector<char> &output) {
    // just return the original pixels
    output = input;
    return true;
}