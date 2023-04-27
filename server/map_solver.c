#include "map_solver.h"

/**
 * map solver
 * @param input[in] the input of the map data
 * @param solutionp[out] a pointer to the buffer to store the solution (needs to be allocated in this function)
 * @param solution_lenp[out] a pointer to the length of the solution string (whose value needs to be updated in this function)
 */
bool solveMap(char *input, char **solutionp, size_t *solution_lenp) {
    char *itr = input, *mark = input, *input = NULL;
    while (*itr != '\n')    itr ++;
    *itr = 0;
    int w = atoi(mark);
    itr ++;
    mark = itr;
    while (*itr != '\n')    itr ++;
    *itr = 0;
    int h = atoi(mark);
    itr ++;
    input = itr;

    *solution_lenp = w * h * 4;
    return solveMapHelper(w, h, input, solutionp);
}


/**
 * map solver
 * @param w[in] the width of the image
 * @param h[in] the height of the image
 * @param input[in] the array of original pixels
 * @param output[in] the pointer to an array of updated pixels (needs to be dynamically allocated)
 */
bool solveMapHelper(int w, int h, char *input, char **output) {
    // just return the original pixels
    size_t length = w * h * 4;
    *output = (char *) Malloc(length);
    
    memcpy(*output, input, length);
    return true;
}