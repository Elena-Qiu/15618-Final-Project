#include <stdio.h>
#include <stdlib.h>

int w = 20;
int GRID_DIM = 8;

int getGridIdxX(int globalX) {
    int quotient = w / GRID_DIM;
    int remainder = w % GRID_DIM;
    int division_point = (GRID_DIM - remainder) * quotient - 1;

    if (globalX <= division_point) {
        return globalX / quotient;
    }
    int extra = globalX - division_point - 1;
    int idxExtra = extra / (quotient + 1);
    return GRID_DIM - remainder + idxExtra;
}

int getGridWidth(int gridIdxX) {
    int quotient = w / GRID_DIM;
    int remainder = w % GRID_DIM;
    if (gridIdxX < GRID_DIM - remainder)
        return quotient;
    else
        return quotient + 1;
}

int main() {
    for (int i = 0; i < GRID_DIM; ++i) {
        printf("%d: %d\n", i, getGridWidth(i));
    }
}