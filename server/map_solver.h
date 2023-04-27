#ifndef MAP_SOLVER_H
#define MAP_SOLVER_H

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "csapp.h"

bool solveMap(char *input, char **solutionp, size_t *solution_lenp);
bool solveMapHelper(int w, int h, char *input, char **output);

#endif
