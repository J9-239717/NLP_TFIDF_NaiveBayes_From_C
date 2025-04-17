#ifndef _SPARSE_MATRIX_H
#define _SPARSE_MATRIX_H

#include "import.h"
#include "structure_data.h"
#include "wordhash.h"

#define index(i,j,cols) ((i)*(cols)+(j)) // cols is size of vocab

sparse_matrix* createSparseMatrix(int initial_capacity,int rows, int cols);
int add_entry(sparse_matrix* matrix, int row, int col, float new_value);
int delete_entry(sparse_matrix* matrix, int row, int col);
void printSparseMatrix(sparse_matrix* matrix);
void freeSparseMatrix(sparse_matrix* matrix);
sparse_matrix* sparese_row_wise(sparse_matrix* matrix,float* vector,int size_vector);
void print_SP_Matrix_to_file(sparse_matrix* matrix, const char* filename,word_hash* vocab);
csr_matrix* to_csr(sparse_matrix* sm);
csc_matrix* to_csc(sparse_matrix* sm);
csr_matrix* csr_transpose(csr_matrix* ori);
void free_csr_matrix(csr_matrix* csr);
void free_csc_matrix(csc_matrix* csc);
float** csr_to_dense(csr_matrix* csr);
float** csc_to_dense(csc_matrix* csc);
void free_dense_matrix(float** dense, int rows);
void print_dense_matrix(float** dense, int rows, int cols);
void print_csc(csc_matrix* csc);
void print_csr(csr_matrix* csr);
csc_matrix* covert_to_csc(csr_matrix* ori);
float **csr_x_csc_to_dense(const csr_matrix *A, const csc_matrix *B);
csr_matrix* flat_mt_to_csr(float* arrayMT,int rows,int cols);

#endif