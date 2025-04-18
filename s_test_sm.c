#include "sparse_matrix.h"

int main(int argc, char* argv[]) {
    // Create a sparse matrix
    sparse_matrix* sm = createSparseMatrix(10, 4, 3);
    if (!sm) {
        fprintf(stderr, "Failed to create sparse matrix\n");
        return -1;
    }

    // create matrix
    // [1,0,2]
    // [3,0,0]
    // [0,6,7]
    // [7,0,9]
    add_entry(sm, 0, 0, 1.0);
    add_entry(sm, 0, 2, 2.0);
    add_entry(sm, 1, 0, 3.0);
    add_entry(sm, 2, 1, 6.0);
    add_entry(sm, 2, 2, 7.0);
    add_entry(sm, 3, 0, 7.0);
    add_entry(sm, 3, 2, 9.0);

    // Print the sparse matrix
    info_printf("Sparse Matrix:\n");
    printSparseMatrix(sm);

    csr_matrix* csr = to_csr(sm);
    info_printf("CSR Matrix:\n");
    print_csr(csr);
    info_printf("to dense matrix:\n");
    float** dense = csr_to_dense(csr);
    print_dense_matrix(dense, csr->rows, csr->cols);

    info_printf("Transpose CSR to CSR_T:\n");
    csr_matrix* csr_t = csr_transpose(csr);
    print_csr(csr_t);
    info_printf("to dense matrix:\n");
    float** dense_transposed_csc = csr_to_dense(csr_t);
    print_dense_matrix(dense_transposed_csc, csr_t->rows, csr_t->cols);

    info_printf("convert CSR to CSC of csr_t:\n");
    csc_matrix* csc = covert_to_csc(csr_t);
    print_csc(csc);
    info_printf("to dense matrix:\n");
    float** csc_dense = csc_to_dense(csc);
    print_dense_matrix(csc_dense, csc->rows, csc->cols);


    info_printf("Dot product of CSR and CSC:\n");
    float** result = csr_x_csc_to_dense(csr, csc);
    print_dense_matrix(result, csr->rows, csc->cols);


    // Free the dense matrix
    free_dense_matrix(result, csr->rows);
    free_dense_matrix(dense, csr->rows);
    free_dense_matrix(dense_transposed_csc, csr_t->rows);
    free_dense_matrix(csc_dense, csc->rows);
    freeSparseMatrix(sm);
    free_csr_matrix(csr);
    free_csr_matrix(csr_t);
    free_csc_matrix(csc);

    int rows = 4;
    int cols = 3;

    float arrayMT[]={1.0f,2.0f,3.0f,
                    3.0f,0.0f,5.0f,
                    5.0f,6.0f,7.0f,
                    7.0f,8.0f,199.0f};

    // Convert the flat array to CSR format
    csr_matrix* csr_from_array = flat_mt_to_csr(arrayMT, rows, cols);
    info_printf("CSR from flat array:\n");
    print_csr(csr_from_array);
    info_printf("to dense matrix:\n");
    float** dense_from_array = csr_to_dense(csr_from_array);
    print_dense_matrix(dense_from_array, csr_from_array->rows, csr_from_array->cols);

    // Free the dense matrix
    free_dense_matrix(dense_from_array, csr_from_array->rows);
    free_csr_matrix(csr_from_array);
    return 0;
}