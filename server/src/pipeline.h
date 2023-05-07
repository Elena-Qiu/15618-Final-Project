#ifndef MAP_SOLVER_H
#define MAP_SOLVER_H

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <string>

#include "csapp.h"

bool solveMap(char *input, char **solutionp, size_t *solution_lenp);
bool solveMapHelper(bool seq, int w, int h, const std::vector<int> &input, std::vector<int> &output);

#endif
