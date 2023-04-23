#include "map_solver.h"

/**
 * map solver
 * @param input[in] the input of the map data
 * @param solutionp[out] a pointer to the buffer to store the solution (needs to be allocated in this function)
 * @param solution_lenp[out] a pointer to the length of the solution string (whose value needs to be updated in this function)
 */
bool solveMap(char *input, char **solutionp, size_t *solution_lenp) {
    char *str = "hello from server!\n";

    *solution_lenp = strlen(str);
    
    *solutionp = (char *) Malloc(*solution_lenp + 1);
    strcpy(*solutionp, str);

    return true;
}