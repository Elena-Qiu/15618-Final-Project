#include "map_solver.h"


/**
 * convert a string representing nodes map values to an int array
 */
static void constructInputArray(char *input, std::vector<int> &array) {
    char *itr = input, *mark = input;
    char delimeter = ',';
    unsigned counter = 0;
    while (counter < array.size()) {
        while (*itr != delimeter)   itr ++;
        *itr = 0;

        array[counter++] = atoi(mark);

        itr ++;
        mark = itr;
    }
}


/**
 * convert an int array to a string representing nodes map
 */
static void constructOutputArray(std::vector<int> &array, char **output, size_t *solution_lenp) {
    std::string output_str;
    for (int val : array)
        output_str += std::to_string(val) + ",";

    // get rid of the last comma
    size_t length = output_str.size() - 1;
    *solution_lenp = length;
    char *solution_cstr = (char *) output_str.c_str();
    *output = (char *) Malloc(length);
    memcpy(*output, solution_cstr, length);
}


/**
 * map solver
 * @param input[in] the input of the map data
 * @param solutionp[out] a pointer to the buffer to store the solution (needs to be allocated in this function)
 * @param solution_lenp[out] a pointer to the length of the solution string (whose value needs to be updated in this function)
 */
bool solveMap(char *input, char **solutionp, size_t *solution_lenp) {
    char *itr = input, *mark = input, *pixels_str = NULL;
    while (*itr != '\n')    itr ++;
    *itr = 0;
    int w = atoi(mark);
    itr ++;
    mark = itr;
    while (*itr != '\n')    itr ++;
    *itr = 0;
    int h = atoi(mark);
    itr ++;
    nodes_map_str = itr;

    sio_printf("width = %d, height = %d\n", w, h);

    size_t length = w * h;
    std::vector<int> input_pixels(length);
    std::vector<int> output_pixels(length);
    constructInputArray(nodes_map_str, input_pixels);

    bool res = solveMapHelper(w, h, input_pixels, output_pixels);

    if (res)    constructOutputArray(output_pixels, solutionp, solution_lenp);
    return res;
}