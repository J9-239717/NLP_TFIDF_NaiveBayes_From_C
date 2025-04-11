#include "sparse_matrix.h"

sparse_matrix* createSparseMatrix(int initial_capacity,int rows, int cols){
    sparse_matrix* matrix = (sparse_matrix*)malloc(sizeof(sparse_matrix));
    if (!matrix) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    matrix->data = (sparse_entry*)malloc(initial_capacity * sizeof(sparse_entry));
    if (!matrix->data) {
        fprintf(stderr, "Memory allocation failed\n");
        free(matrix);
        return NULL;
    }
    matrix->capacity = initial_capacity;
    matrix->size = 0;
    matrix->rows = rows;
    matrix->cols = cols;
    return matrix;
}

int add_entry(sparse_matrix* matrix, int row, int col, float new_value){
    if(row >= matrix->rows || col >= matrix->cols){
        fprintf(stderr, "Row or column index out of bounds\n");
        return -1;
    }

    // ## TODO: check this function who used it not sure it have exist row or col , it should check frist call this function becuaes if check in this it's slow
    // // Check if the entry already exists
    // for (int i = 0; i < matrix->size; i++) {
    //     if (matrix->data[i].row == row && matrix->data[i].col == col) {
    //         // Update the existing entry
    //         matrix->data[i].value = new_value;
    //         return 0;
    //     }
    // }
    // If the entry does not exist, add a new one
    if (matrix->size >= matrix->capacity) {
        // Resize the array if necessary
        matrix->capacity *= 2;
        matrix->data = (sparse_entry*)realloc(matrix->data, matrix->capacity * sizeof(sparse_entry));
        if (!matrix->data) {
            fprintf(stderr, "Memory allocation failed\n");
            return -1;
        }
        printf("Reallocating memory for sparse matrix\n");
    }

    // Add the new entry
    matrix->data[matrix->size++] = (sparse_entry){row, col, new_value};
    return 0;
}

int delete_entry(sparse_matrix* matrix, int row, int col){
    if(row >= matrix->rows || col >= matrix->cols){
        fprintf(stderr, "Row or column index out of bounds\n");
        return -1;
    }

    // Find the entry to delete
    for (int i = 0; i < matrix->size; i++) {
        if (matrix->data[i].row == row && matrix->data[i].col == col) {
            // Shift the remaining entries to fill the gap
            for (int j = i; j < matrix->size - 1; j++) {
                matrix->data[j] = matrix->data[j + 1];
            }
            matrix->size--;
            return 0;
        }
    }
}

void printSparseMatrix(sparse_matrix* matrix){
    printf("Sparse Matrix:\n");
    for (int i = 0; i < matrix->size; i++) {
        printf("%d.Doc %d - Word %d = %.2f\n",i ,matrix->data[i].row, matrix->data[i].col, matrix->data[i].value);
    }
}
void freeSparseMatrix(sparse_matrix* matrix){
    if (matrix) {
        free(matrix->data);
        free(matrix);
    }
}
sparse_matrix* sparese_row_wise(sparse_matrix* matrix,float* vector,int size_vector){
    if(size_vector != matrix->cols){
        fprintf(stderr, "Size of vector and matrix cols are not equal\n");
        fprintf(stderr, "Size of vector: %d and matrix cols: %d\n", size_vector, matrix->cols);
        return NULL;
    }

    sparse_matrix* result = createSparseMatrix(matrix->size, matrix->rows, matrix->cols);
    if (!result) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    for(int i = 0; i < matrix->size; i++){
        int col = matrix->data[i].col;
        float value = matrix->data[i].value * vector[col];
        add_entry(result, matrix->data[i].row, col, value);
    }
    return result;
}