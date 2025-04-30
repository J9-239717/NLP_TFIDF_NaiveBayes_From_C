#include "h_files/sparse_matrix.h"

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

void print_SP_Matrix_to_file(sparse_matrix* matrix, const char* filename,word_hash* vocab){
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Cannot open file %s\n", filename);
        return;
    }
    for (int i = 0; i < matrix->size; i++) {
        fprintf(file, "%d.Doc %d - Word %s = %.2f\n",i ,matrix->data[i].row,getWordFromIndex(vocab,matrix->data[i].col), matrix->data[i].value);
    }
    fclose(file);
}

csr_matrix* to_csr(sparse_matrix* sm){
    // parse the sparse matrix into CSR format

    csr_matrix* csr = (csr_matrix*)malloc(sizeof(csr_matrix));
    checkExistMemory(csr);
    
    // intialize the CSR matrix size
    csr->rows = sm->rows;
    csr->cols = sm->cols;
    csr->nnz = sm->size;

    // allocate memory for the CSR matrix
    csr->values = (float*)malloc(csr->nnz * sizeof(float));
    csr->col_index = (int*)malloc(csr->nnz * sizeof(int));
    csr->row_ptr = (int*)calloc((csr->rows + 1) ,sizeof(int));
    checkExistMemory(csr->values);
    checkExistMemory(csr->col_index);
    checkExistMemory(csr->row_ptr);

    // 1. Count non-zero elements in each row
    for(int i = 0; i < sm->size; i++){
        csr->row_ptr[sm->data[i].row + 1]++;
    }

    // 2. Cumulative sum
    for(int i = 1; i <= csr->rows; i++){
        csr->row_ptr[i] += csr->row_ptr[i - 1];
    }

    // 3. Fill the values and column indices
    int* current_row_ptr = (int*)calloc(csr->rows ,sizeof(int));
    checkExistMemory(current_row_ptr);
    int row = 0,idx = 0;
    for(int i = 0; i < sm->size; i++){
        row = sm->data[i].row;
        idx = csr->row_ptr[row] + current_row_ptr[row]++;
        csr->values[idx] = sm->data[i].value;
        csr->col_index[idx] = sm->data[i].col;
    }
    free(current_row_ptr);
    return csr;
}

csc_matrix* to_csc(sparse_matrix* sm){
    // parse the sparse matrix into CSC format

    csc_matrix* csc = (csc_matrix*)malloc(sizeof(csc_matrix));
    checkExistMemory(csc);
    
    // intialize the CSC matrix size
    csc->rows = sm->rows;
    csc->cols = sm->cols;
    csc->nnz = sm->size;

    // allocate memory for the CSC matrix
    csc->values = (float*)malloc(csc->nnz * sizeof(float));
    csc->row_index = (int*)malloc(csc->nnz * sizeof(int));
    csc->col_ptr = (int*)calloc((csc->cols + 1) ,sizeof(int));
    checkExistMemory(csc->values);
    checkExistMemory(csc->row_index);
    checkExistMemory(csc->col_ptr);

    // 1. Count non-zero elements in each column
    for(int i = 0; i < sm->size; i++){
        csc->col_ptr[sm->data[i].col + 1]++;
    }

    // 2. Cumulative sum
    for(int i = 1; i <= csc->cols; i++){
        csc->col_ptr[i] += csc->col_ptr[i - 1];
    }

    // 3. Fill the values and column indices
    int* current_col_ptr = (int*)calloc(csc->cols ,sizeof(int));
    checkExistMemory(current_col_ptr);
    int col = 0,idx = 0;
    for(int i = 0; i < sm->size; i++){
        col = sm->data[i].col;
        idx = csc->col_ptr[col] + current_col_ptr[col]++;
        csc->values[idx] = sm->data[i].value;
        csc->row_index[idx] = sm->data[i].row;
    }
    free(current_col_ptr);
    return csc;
}

// Function transpose csr to csr_t
csr_matrix* csr_transpose(csr_matrix* ori){
    checkExistMemory(ori);

    int rows = ori->cols; // Transpose rows and columns
    int cols = ori->rows; // Transpose rows and columns
    int nnz = ori->nnz;

    csr_matrix* csr = (csr_matrix*)malloc(sizeof(csr_matrix));
    checkExistMemory(csr);
    csr->rows = rows;
    csr->cols = cols;
    csr->nnz = nnz;

    csr->values = (float*)malloc(nnz * sizeof(float));
    csr->col_index = (int*)malloc(nnz * sizeof(int));
    csr->row_ptr = (int*)calloc((rows + 1) ,sizeof(int));

    checkExistMemory(csr->values);
    checkExistMemory(csr->col_index);
    checkExistMemory(csr->row_ptr);

    // Count non-zero elements in each row
    for(int i = 0; i < nnz; i++){
        csr->row_ptr[ori->col_index[i] + 1]++;
    }

    // Cumulative sum
    for(int i = 1; i <= rows; i++){
        csr->row_ptr[i] += csr->row_ptr[i - 1];
    }

    // Fill the values and column indices
    int* current_row_ptr = (int*)calloc(rows ,sizeof(int));
    checkExistMemory(current_row_ptr);
    for(int slide = 0; slide < cols; slide++){
        int start = ori->row_ptr[slide];
        int end = ori->row_ptr[slide + 1];
        for(int i = start; i < end; i++){
            float value = ori->values[i];
            int new_row = ori->col_index[i];
            int new_row_ptr = csr->row_ptr[new_row] + current_row_ptr[new_row]++;

            csr->values[new_row_ptr] = value;
            csr->col_index[new_row_ptr] = slide;
        }
    }
    free(current_row_ptr);
    return csr;
}

csc_matrix* covert_to_csc(csr_matrix* ori){
    checkExistMemory(ori);

    // not transpose the matrix
    int rows  = ori->rows;
    int cols  = ori->cols;
    int nnz   = ori->nnz;

    csc_matrix* csc = (csc_matrix*)malloc(sizeof(csc_matrix));
    checkExistMemory(csc);

    csc->rows = rows;
    csc->cols = cols;
    csc->nnz = nnz;

    csc->values = (float*)malloc(nnz * sizeof(float));
    csc->row_index = (int*)malloc(nnz * sizeof(int));
    csc->col_ptr = (int*)calloc((cols + 1) ,sizeof(int));
    checkExistMemory(csc->values);
    checkExistMemory(csc->row_index);
    checkExistMemory(csc->col_ptr);

    // Count non-zero elements in each column
    for(int i = 0; i < nnz; i++){
        csc->col_ptr[ori->col_index[i] + 1]++;
    }

    // Cumulative sum
    for(int i = 1; i <= cols; i++){
        csc->col_ptr[i] += csc->col_ptr[i - 1];
    }

    // Fill the values and column indices
    int* current_col_ptr = (int*)calloc(cols ,sizeof(int));
    checkExistMemory(current_col_ptr);
    for(int slide = 0; slide < rows; slide++){
        int start = ori->row_ptr[slide];
        int end = ori->row_ptr[slide + 1];
        for(int i = start; i < end; i++){
            float value = ori->values[i];
            int new_col = ori->col_index[i];
            int new_col_ptr = csc->col_ptr[new_col] + current_col_ptr[new_col]++;

            csc->values[new_col_ptr] = value;
            csc->row_index[new_col_ptr] = slide;
        }
    }
    free(current_col_ptr);
    return csc;
}

void free_csr_matrix(csr_matrix* csr){
    if (csr) {
        free(csr->values);
        free(csr->col_index);
        free(csr->row_ptr);
        free(csr);
    }
}

void free_csc_matrix(csc_matrix* csc){
    if (csc) {
        free(csc->values);
        free(csc->row_index);
        free(csc->col_ptr);
        free(csc);
    }
}

float** csr_to_dense(csr_matrix* csr) {
    if (!csr) return NULL;

    float** dense = (float**)malloc(sizeof(float*) * csr->rows);
    for (int i = 0; i < csr->rows; i++) {
        dense[i] = (float*)calloc(csr->cols, sizeof(float));
    }

    for (int i = 0; i < csr->rows; i++) {
        int start = csr->row_ptr[i];
        int end = csr->row_ptr[i + 1];
        for (int j = start; j < end; j++) {
            int col = csr->col_index[j];
            if(col >= csr->cols) {
                fprintf(stderr, "Column index out of bounds\n");
                free_dense_matrix(dense, csr->rows);
                return NULL;
            }
            dense[i][col] = csr->values[j];
        }
    }

    return dense;
}


float** csc_to_dense(csc_matrix* csc) {
    if (!csc) return NULL;

    float** dense = (float**)malloc(sizeof(float*) * csc->rows);
    for (int i = 0; i < csc->rows; i++) {
        dense[i] = (float*)calloc(csc->cols, sizeof(float));
    }

    for (int i = 0; i < csc->cols; i++) {
        int start = csc->col_ptr[i];
        int end = csc->col_ptr[i + 1];
        for (int j = start; j < end; j++) {
            int row = csc->row_index[j];
            dense[row][i] = csc->values[j];
        }
    }

    return dense;
}

void free_dense_matrix(float** dense, int rows) {
    if (dense) {
        for (int i = 0; i < rows; i++) {
            free(dense[i]);
        }
        free(dense);
    }
}

void print_dense_matrix(float** dense, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        printf("[ ");
        for (int j = 0; j < cols; j++) {
            printf("%.2f,", dense[i][j]);
        }
        printf("]\n");
    }
}

void print_csr(csr_matrix* csr) {
    if (!csr) return;

    printf("CSR Matrix:\n");
    printf("Rows: %d, Cols: %d, Non-zeros: %d\n", csr->rows, csr->cols, csr->nnz);

    // Print values
    printf("Values: ");
    for (int i = 0; i < csr->nnz; i++) {
        printf("%.2f ", csr->values[i]);
    }
    printf("\n");

    // Print column indices
    printf("Column indices: ");
    for (int i = 0; i < csr->nnz; i++) {
        printf("%d ", csr->col_index[i]);
    }
    printf("\n");

    // Print row pointers
    printf("Row pointers: ");
    for (int i = 0; i <= csr->rows; i++) {
        printf("%d ", csr->row_ptr[i]);
    }
    printf("\n");
}

void print_csc(csc_matrix* csc) {
    if (!csc) return;

    printf("CSC Matrix:\n");
    printf("Rows: %d, Cols: %d, Non-zeros: %d\n", csc->rows, csc->cols, csc->nnz);

    // Print values
    printf("Values: ");
    for (int i = 0; i < csc->nnz; i++) {
        printf("%.2f ", csc->values[i]);
    }
    printf("\n");

    // Print row indices
    printf("Row indices: ");
    for (int i = 0; i < csc->nnz; i++) {
        printf("%d ", csc->row_index[i]);
    }
    printf("\n");

    // Print column pointers
    printf("Column pointers: ");
    for (int i = 0; i <= csc->cols; i++) {
        printf("%d ", csc->col_ptr[i]);
    }
    printf("\n");
}

static inline float dot_csr_row_csc_col(const csr_matrix *A, int row,const csc_matrix *B, int col)
{
    int pa = A->row_ptr[row];
    int pa_end = A->row_ptr[row + 1];
    int pb = B->col_ptr[col];
    int pb_end = B->col_ptr[col + 1];
    float sum = 0.0f;
    while (pa < pa_end && pb < pb_end) {
        int ia = A->col_index[pa];   
        int ib = B->row_index[pb];   
        if (ia == ib) {
            sum += A->values[pa] * B->values[pb];
            ++pa; ++pb;
        } else if (ia < ib) {
            ++pa;
        } else {
            ++pb;
        }
    }
    return sum;
}

float **csr_x_csc_to_dense(const csr_matrix *A, const csc_matrix *B){
    if (!A || !B) {
        fprintf(stderr, "NULL matrix pointer.\n");
        return NULL;
    }
    if (A->cols != B->rows) {
        fprintf(stderr, "Dimension mismatch: %dx%d  *  %dx%d\n", A->rows, A->cols, B->rows, B->cols);
        return NULL;
    }

    float **C = (float **)malloc(A->rows * sizeof(float *));
    if (!C) {
        perror("malloc");
        return NULL;
    }

    for (int i = 0; i < A->rows; ++i) {
        C[i] = (float *)calloc(B->cols, sizeof(float));
        if (!C[i]) { 
            perror("calloc");
            for (int k = 0; k < i; ++k) free(C[k]);
            free(C);
            return NULL;
        }
    }
    for (int i = 0; i < A->rows; ++i) {
        for (int j = 0; j < B->cols; ++j) {
            C[i][j] = dot_csr_row_csc_col(A, i, B, j);
        }
    }
    return C;
}

// !!should check dont have any 0 value in flat matrix
// this function not handle 0 value in matrix
// if have 0 value in matrix, it will store in csr matrix
// and it will be wrong with sparse matrix to not store 0 values
csr_matrix* flat_mt_to_csr(float* arrayMT,int rows,int cols){
    csr_matrix* csr = (csr_matrix*)malloc(sizeof(csr_matrix));
    checkExistMemory(csr);
    
    // intialize the CSR matrix size
    csr->rows = rows;
    csr->cols = cols;
    csr->nnz = rows * cols;

    // allocate memory for the CSR matrix
    csr->values = (float*)malloc(csr->nnz * sizeof(float));
    csr->col_index = (int*)malloc(csr->nnz * sizeof(int));
    csr->row_ptr = (int*)calloc((rows + 1) ,sizeof(int));
    checkExistMemory(csr->values);
    checkExistMemory(csr->col_index);
    checkExistMemory(csr->row_ptr);

    // 1. Count non-zero elements in each row
    for(int i = 0; i < rows; i++){
        csr->row_ptr[i + 1] = cols;
        for(int j = 0; j < cols; j++){
            int index = index(i,j,cols);
            csr->values[index] = arrayMT[index];
            csr->col_index[index] = j;
        }
    }

    for(int i = 0; i < rows; i++){
        csr->row_ptr[i + 1] += csr->row_ptr[i];
    }

    return csr;
}