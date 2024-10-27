#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define MATRIX_SIZE 1024

double matrix[MATRIX_SIZE][MATRIX_SIZE];
double result[MATRIX_SIZE][MATRIX_SIZE];

void initialize_matrix(void) {
    for (int i = 0; i < MATRIX_SIZE; ++i)
        for (int j = 0; j < MATRIX_SIZE; ++j)
            matrix[i][j] = (double)rand() / RAND_MAX;
}

double get_time(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void row_sequential_access(void) {
    for (int i = 0; i < MATRIX_SIZE; ++i)
        for (int j = 0; j < MATRIX_SIZE; ++j)
            result[i][j] = matrix[i][j] * 2.0;
}

void column_access(void) {
    for (int j = 0; j < MATRIX_SIZE; ++j)
        for (int i = 0; i < MATRIX_SIZE; ++i)
            result[i][j] = matrix[i][j] * 2.0;
}

void random_access(int *row_indices, int *col_indices) {
    for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; ++i) {
        int row = row_indices[i];
        int col = col_indices[i];
        result[row][col] = matrix[row][col] * 2.0;
    }
}

int main(void) {
    srand((uint)time(NULL));
    initialize_matrix();

    double start, end;

    start = get_time();
    row_sequential_access();
    end = get_time();
    printf("Sequential Access Time: %f seconds\n", end - start);

    start = get_time();
    column_access();
    end = get_time();
    printf("Column Access with Stride Time: %f seconds\n", end - start);

    int *row_indices = malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(int));
    int *col_indices = malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(int));
    for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; ++i) {
        row_indices[i] = rand() % MATRIX_SIZE;
        col_indices[i] = rand() % MATRIX_SIZE;
    }

    start = get_time();
    random_access(row_indices, col_indices);
    end = get_time();
    printf("Random Access Time: %f seconds\n", end - start);

    free(row_indices);
    free(col_indices);

    return 0;
}
