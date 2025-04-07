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

typedef struct _word_node{
    char* word;
    int freq;
    struct _word_node* next;
}word_node;

typedef struct _word_hash{
    word_node* table[26]; // array store word a - z
    int size_each[26]; // size of each hash table a - z
    int size; // size of hash table
}word_hash;

typedef struct _sparse_node{
    int row;
    int col;
    double values;
}sparse_node;

typedef struct _sparse_matrix{
    sparse_node* data; // array of sparse node
    int size; // size of array of sparse node
    int rows, cols;
} sparse_matrix;

#endif