#ifndef _STRUCTURE_DATA_H_
#define _STRUCTURE_DATA_H_

typedef struct _label_frequency{
    char* label;
    int frequency;
    struct _label_frequency* next;
}label_frequency;

typedef struct _data_frame_node{
    char* text;
    int count_word;
    char* label;
}_data_frame_node;

typedef struct _data_frame{
    char** keys; // header name
    int sizeKeys; // size of header name
    label_frequency* label_freq; // label frequency
    _data_frame_node* data; // array of data frame
    int size; // size of array of data frame
}data_frame;

typedef struct _String_Pool{
    char* buffer;
    int used;
    int capacity;
}StringPool;

typedef struct _word_node{
    char* word;
    int freq;
    struct _word_node* next;
}word_node;

typedef struct _word_node_pool{
    word_node** pool;
    int used_block;
    int block_size;
    int used;
}word_node_pool;

typedef struct _word_hash{
    word_node* table[26]; // array store word a - z
    int size_each[26]; // size of each hash table a - z
    int size; // size of hash table
    StringPool* str_pool; // string pool
    word_node_pool* node_pool; // node pool
}word_hash;

typedef struct {
    int row;
    int col;
    float value;
} sparse_entry;

typedef struct {
    sparse_entry* data;
    int size; // real size of the sparse matrix
    int capacity; // capacity of the sparse matrix for growth
    int rows;
    int cols;
} sparse_matrix;

typedef struct _TF_IDF_OBJECT{
    data_frame* df; // document for train
    word_hash* hash; // hash table for vocab
    float* idf_vector; // idf vector
    sparse_matrix* tf_idf_matrix; // tf matrix
}TF_IDF_OJ;

#endif