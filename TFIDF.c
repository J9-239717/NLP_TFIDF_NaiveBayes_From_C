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
            float idf = ((float) (N + alpha) / (float) (node->freq + alpha));
            tfidf->idf_vector[count++] = log(idf) + alpha;
            node = node->next;
        }
    }
    return count;
}

sparse_matrix* compute_tf(TF_IDF_OJ* tfidf,data_frame* df,int alpha,int ngram){
    sparse_matrix* matrix = createSparseMatrix(1024*1024, df->size, tfidf->hash->size);
    checkExistMemory(matrix);
    int N = df->size; // number of documents
    word_hash* temp = createWordHash();
    int count_word = 0;
    int col=0,row=0;
    float value = 0;
    int count_miss = 0;
    StringPool* str_pool = create_string_pool();
    word_node* node = NULL;
    for(int i = 0; i < N; i++){
        String_Ngram(temp,df->data[i].text, ngram, str_pool);
        checkExistMemory(temp);
        count_word = temp->size;
        node = NULL;
        for(int j = 0 ; j < WORD_HASH_SIZE; j++){
            node = temp->table[j];
            while(node){
                col = getIndexOfWord(tfidf->hash, node->word);
                row = i;
                if(col == -1){
                    count_miss++;
                    node = node->next;
                    continue;
                }
                value = (float) ((node->freq + alpha) / ((float)count_word + alpha));
                add_entry(matrix, row, col, value);
                node = node->next;
            }
        }
        reset_string_pool(temp->str_pool);
        reset_word_node_pool(temp->node_pool);
        resetWordHash(temp);
    }
    destroy_string_pool(str_pool);
    freeWordHash(temp);
    info_printf("Count miss: %d\n", count_miss);
    return matrix;
}

sparse_matrix* compute_tf_idf(sparse_matrix* tf_matrix,float* idf_vector,int size_vector){
    sparse_matrix* temp = sparese_row_wise(tf_matrix, idf_vector, size_vector);
    checkExistMemory(temp);
    return temp;
}

sparse_matrix* fit_transform(TF_IDF_OJ* tfidf,int ngram){
    checkExistMemory(tfidf);
    func_printf("build vocab\n");
    start_timer();
    build_vocab(tfidf, ngram);
    show_time();

    tfidf->idf_vector = (float*)malloc(sizeof(float) * tfidf->hash->size);
    func_printf("compute idf\n");
    start_timer();
    compute_idf(tfidf, ALPHA);
    show_time();

    func_printf("compute tf\n");
    start_timer();
    sparse_matrix* tf = compute_tf(tfidf, tfidf->df,ALPHA, ngram);
    checkExistMemory(tf);
    show_time();

    func_printf("compute tf-idf\n");
    start_timer();
    tfidf->tf_idf_matrix = compute_tf_idf(tf, tfidf->idf_vector, tfidf->hash->size);
    show_time();
    freeSparseMatrix(tf);
    return tfidf->tf_idf_matrix;
}

void fit_tfidf(TF_IDF_OJ* tfidf,int ngram){
    checkExistMemory(tfidf);
    build_vocab(tfidf, ngram);

    tfidf->idf_vector = (float*)malloc(sizeof(float) * tfidf->hash->size);
    compute_idf(tfidf, ALPHA);

    sparse_matrix* tf = compute_tf(tfidf, tfidf->df,ALPHA, ngram);
    checkExistMemory(tf);

    tfidf->tf_idf_matrix = compute_tf_idf(tf, tfidf->idf_vector, tfidf->hash->size);
    freeSparseMatrix(tf);
}

TF_IDF_OJ* transform(TF_IDF_OJ* original,data_frame* df,int ngram){
    checkExistMemory(original);
    TF_IDF_OJ* rt = createTF_IDF(df);
    checkExistMemory(rt);
    rt->hash = original->hash;
    rt->idf_vector = original->idf_vector;
    sparse_matrix* tf = compute_tf(original, df, ALPHA, ngram);
    checkExistMemory(tf);
    sparse_matrix* temp = compute_tf_idf(tf, original->idf_vector, original->hash->size);
    checkExistMemory(temp);
    rt->tf_idf_matrix = temp;
    freeSparseMatrix(tf);
    return rt;
}

void freeTF_IDF(TF_IDF_OJ* tfidf){
    if(!tfidf){
        fprintf(stderr, "TF-IDF is NULL\n");
        return;
    }
    if(tfidf->hash){
        freeWordHash(tfidf->hash);
        tfidf->hash = NULL;
    }
    if(tfidf->idf_vector){
        free(tfidf->idf_vector);
        tfidf->idf_vector = NULL;
    }
    if(tfidf->tf_idf_matrix){
        freeSparseMatrix(tfidf->tf_idf_matrix);
        tfidf->tf_idf_matrix = NULL;
    }
    free(tfidf);
}

void printTF_IDF(TF_IDF_OJ* tfidf){
    if(!tfidf){
        fprintf(stderr, "TF-IDF is NULL\n");
        return;
    }
    info_printf("Vocab:\n");
    printWordHash(tfidf->hash);
    info_printf("TF-IDf with N doc is %d\n",tfidf->df->size);
    sparse_matrix* matrix = tfidf->tf_idf_matrix;
    for (int i = 0; i < matrix->size; i++) {
        printf("  %d.Doc %d - Word %s = %.2f\n",i ,matrix->data[i].row, getWordFromIndex(tfidf->hash,matrix->data[i].col), matrix->data[i].value);
    }
    info_printf("IDF\n");
    for(int i = 0; i < tfidf->hash->size; i++){
        printf("  %s:%f\n",getWordFromIndex(tfidf->hash,i) ,tfidf->idf_vector[i]);
    }
    info_printf("\n");
    info_printf("Size of vocab: %d\n", tfidf->hash->size);
    info_printf("Size of tf-idf matrix: %d\n", tfidf->tf_idf_matrix->size);
}