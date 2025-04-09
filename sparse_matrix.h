#ifndef _SPARSE_MATRIX_H
#define _SPARSE_MATRIX_H

#include "import.h"
#include "structure_data.h"

sparse_matrix* createSparseMatrix(int initial_capacity,int rows, int cols);
int add_entry(sparse_matrix* matrix, int row, int col, float new_value);
int delete_entry(sparse_matrix* matrix, int row, int col);
void printSparseMatrix(sparse_matrix* matrix);
void freeSparseMatrix(sparse_matrix* matrix);
sparse_matrix* sparese_row_wise(sparse_matrix* matrix,float* vector,int size_vector);

#endif