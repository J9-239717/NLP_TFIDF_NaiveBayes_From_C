#ifndef _WORDHASH_H
#define _WORDHASH_H

#include "dataframe.h"
#include "import.h"
#include "utf8_vn.h"

#define WORD_HASH_SIZE 26

typedef struct _word_node{
    char* word;
    int freq;
    struct _word_node* next;
}word_node;

typedef struct _word_hash{
    word_node* table[26]; // array store word a - z
    int size; // size of hash table
}word_hash;

word_hash* WordHash(data_frame* df);
void printWordHash(word_hash* hash);
void freeWordHash(word_hash* hash);
word_hash* smooth_word(word_hash* hash,int smallest);
int writeWordHashToFile(word_hash* hash, const char* filename);

#endif