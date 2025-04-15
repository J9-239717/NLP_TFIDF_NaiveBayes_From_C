#ifndef _SPARSE_MATRIX_H
#define _SPARSE_MATRIX_H

#include "import.h"
#include "structure_data.h"
#include "wordhash.h"

sparse_matrix* createSparseMatrix(int initial_capacity,int rows, int cols);
int add_entry(sparse_matrix* matrix, int row, int col, float new_value);
int delete_entry(sparse_matrix* matrix, int row, int col);
void printSparseMatrix(sparse_matrix* matrix);
void freeSparseMatrix(sparse_matrix* matrix);
sparse_matrix* sparese_row_wise(sparse_matrix* matrix,float* vector,int size_vector);
void print_SP_Matrix_to_file(sparse_matrix* matrix, const char* filename,word_hash* vocab);

#endif