#include "TFIDF.h"

// this function is used to create TF_IDF_OJ struct
// it will create a new TF_IDF_OJ struct and initialize it
TF_IDF_OJ* createTF_IDF(data_frame* df){
    checkExistMemory(df);
    TF_IDF_OJ* tfidf = (TF_IDF_OJ*)malloc(sizeof(TF_IDF_OJ));
    checkExistMemory(tfidf);
    tfidf->df = df;
    tfidf->hash = NULL;
    tfidf->idf_vector = NULL;
    tfidf->tf_idf_matrix = NULL;
    return tfidf;
}
int build_vocab(TF_IDF_OJ* tfidf,int ngram){
    tfidf->hash = WordHashWithNgram(tfidf->df, ngram);
    // writeWordHashToFile(tfidf->hash, "assets/debug_hash_word_tfidf.txt");
    checkExistMemory(tfidf->hash);
    return tfidf->hash->size;
}
int compute_idf(TF_IDF_OJ* tfidf,int alpha){
    checkExistMemory(tfidf->idf_vector);
    int N = tfidf->df->size; // number of documents
    int count = 0;
    for(int i = 0; i < WORD_HASH_SIZE; i++){
        word_node* node = tfidf->hash->table[i];
        while(node){
            float idf =(float) ((N + alpha) / ((float)node->freq + alpha));
            tfidf->idf_vector[count++] = log(idf + alpha);
            node = node->next;
        }
    }
    return count;
}

sparse_matrix* compute_tf(TF_IDF_OJ* tfidf,data_frame* df,int alpha,int ngram){
    // FILE* file = fopen("assets/debug_word_tfidf.txt", "w");
    sparse_matrix* matrix = createSparseMatrix(1000, df->size, tfidf->hash->size);
    checkExistMemory(matrix);
    int N = df->size; // number of documents
    word_hash* temp = NULL;
    int count_word = 0;
    int col=0,row=0;
    float value = 0;
    int count_miss = 0;
    for(int i = 0; i < N; i++){
        temp = String_Ngram(df->data[i].text, ngram);
        checkExistMemory(temp);
        count_word = temp->size;
        word_node* node = NULL;
        for(int j = 0 ; j < WORD_HASH_SIZE; j++){
            node = temp->table[j];
            while(node){
                col = getIndexOfWord(tfidf->hash, node->word);
                row = i;
                if(col == -1){
                    count_miss++;
                    //fprintf(file, "Miss word: %s\n", node->word);
                    node = node->next;
                    continue;
                }
                value = (float) ((node->freq + alpha) / ((float)count_word + alpha));
                add_entry(matrix, row, col, value);
                node = node->next;
            }
        }
        freeWordHash(temp);
    }
    printf("Count miss: %d\n", count_miss);//fclose(file);
    return matrix;
}

sparse_matrix* compute_tf_idf(sparse_matrix* tf_matrix,float* idf_vector,int size_vector){
    sparse_matrix* temp = sparese_row_wise(tf_matrix, idf_vector, size_vector);
    checkExistMemory(temp);
    return temp;
}

sparse_matrix* fit_transform(TF_IDF_OJ* tfidf,int ngram){
    checkExistMemory(tfidf);
    printf("build vocab\n");
    start_timer();
    build_vocab(tfidf, ngram);
    show_time();
    tfidf->idf_vector = (float*)malloc(sizeof(float) * tfidf->hash->size);
    printf("compute idf\n");
    start_timer();
    compute_idf(tfidf, ALPHA);
    show_time();
    printf("compute tf\n");
    start_timer();
    sparse_matrix* tf = compute_tf(tfidf, tfidf->df,ALPHA, ngram);
    show_time();
    checkExistMemory(tf);
    printf("compute tf-idf\n");
    start_timer();
    tfidf->tf_idf_matrix = compute_tf_idf(tf, tfidf->idf_vector, tfidf->hash->size);
    show_time();
    freeSparseMatrix(tf);
    return tfidf->tf_idf_matrix;
}
sparse_matrix* transform(TF_IDF_OJ* original,data_frame* df,int ngram){
    checkExistMemory(original);
    sparse_matrix* tf = compute_tf(original, df, ALPHA, ngram);
    checkExistMemory(tf);
    sparse_matrix* temp = compute_tf_idf(tf, original->idf_vector, original->hash->size);
    checkExistMemory(temp);
    freeSparseMatrix(tf);
    return temp;
}

void freeTF_IDF(TF_IDF_OJ* tfidf){
    if(!tfidf){
        fprintf(stderr, "TF-IDF is NULL\n");
        return;
    }
    if(tfidf->hash){
        freeWordHash(tfidf->hash);
    }
    if(tfidf->idf_vector){
        free(tfidf->idf_vector);
    }
    if(tfidf->tf_idf_matrix){
        freeSparseMatrix(tfidf->tf_idf_matrix);
    }
    free(tfidf);
}

void printTF_IDF(TF_IDF_OJ* tfidf){
    if(!tfidf){
        fprintf(stderr, "TF-IDF is NULL\n");
        return;
    }
    printf("Vocab:\n");
    printWordHash(tfidf->hash);
    printf("TF-IDf\n");
    printSparseMatrix(tfidf->tf_idf_matrix);
    printf("IDF\n");
    for(int i = 0; i < tfidf->hash->size; i++){
        printf("%f ", tfidf->idf_vector[i]);
    }
    printf("\n");
    printf("Size of vocab: %d\n", tfidf->hash->size);
    printf("Size of tf-idf matrix: %d\n", tfidf->tf_idf_matrix->size);
}