#ifndef _WORDHASH_H
#define _WORDHASH_H

#include "structure_data.h"
#include "import.h"
#include "utf8_vn.h"
#include "string_pool.h"

#define WORD_HASH_SIZE 27 // 26 letters + 1 for invalid data
#define INITAL_WORD_HASH_SIZE_POOL 10000
#define BLOCK_SIZE_WORD_HASH 3

enum {INVALID_DATA_INDEX = 26}; // invalid data

word_hash* WordHash(data_frame* df);
void printWordHash(word_hash* hash);
void freeWordHash(word_hash* hash);
word_hash* smooth_word(word_hash* hash,int smallest);
int writeWordHashToFile(word_hash* hash, const char* filename);
char** getWordFormHash(word_hash* src,int* rt_size);
int getWordFreq(word_hash* hash, char* word);
int road_word_hash(word_hash* src, const char* filename);
word_hash* createWordHash();
word_hash* WordHashWithNgram(data_frame* df, int n);
int getIndexOfWord(word_hash* hash, char* word);
void String_Ngram(word_hash* origin,char* str_org, int n, StringPool* temp);
word_node_pool* createWordNodePool();
int freeWordNodePool(word_node_pool* pool);
word_node* allocate_node(word_node_pool* pool);
void reset_word_node_pool(word_node_pool* pool);
void resetWordHash(word_hash* hash);
char* getWordFromIndex(word_hash* hash, int index);
word_hash* WordHashWithNgram_MultiThread(data_frame* df,int n);

#endif