#include "functions.h"

int main(void) {
    Matrix M;
    if (create(&M) != 0) {
        fprintf(stderr, "\033[1;31mFailed to create matrix. Exiting.\n\033[0m");
        return EXIT_FAILURE;
    }

    if (inputMatrix(&M) != 0) {
        return EXIT_FAILURE;
    }

    printMatrix(&M);

    Vector b = {0, NULL};
    if (computeVectorB(&M, &b) != 0) {
        eraseMatrix(&M);
        return EXIT_FAILURE;
    }
    printVectorB(&b);

    free(b.data);
    eraseMatrix(&M);
    return EXIT_SUCCESS;
}
