#ifndef _WORDHASH_H
#define _WORDHASH_H

#include "dataframe.h"
#include "import.h"
#include "utf8_vn.h"

typedef struct _word_node{
    char* word;
    int freq;
    struct _word_node* next;
}word_node;

typedef struct _word_hash{
    word_node* table[25]; // array store word a - z
}word_hash;

#endif