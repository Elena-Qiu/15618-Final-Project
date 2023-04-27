#include "map_solver.h"


/**
 * convert a string representing pixel values to an uint8 (i.e. char) array
 */
static void constructInputArray(char *input, std::vector<char> &array) {
    char *itr = input, *mark = input;
    char delimeter = ',';
    unsigned counter = 0;
    while (counter < array.size()) {
        while (*itr != delimeter)   itr ++;
        *itr = 0;

        array[counter++] = (char) atoi(mark);

        itr ++;
        mark = itr;
    }
}


/**
 * convert an uint8 (i.e. char) array to a string representing pixel values
 */
static void constructOutputArray(std::vector<char> &array, char **output, size_t *solution_lenp) {
    std::string output_str;
    for (char val : array)
        output_str += std::to_string((unsigned char) val) + ",";

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
    pixels_str = itr;

    sio_printf("width = %d, height = %d\n", w, h);

    size_t length = w * h * 4;
    std::vector<char> input_pixels(length);
    std::vector<char> output_pixels(length);
    constructInputArray(pixels_str, input_pixels);

    bool res = solveMapHelper(w, h, input_pixels, output_pixels);

    if (res)    constructOutputArray(output_pixels, solutionp, solution_lenp);
    return res;
}