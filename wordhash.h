#ifndef _WORDHASH_H
#define _WORDHASH_H

#include "structure_data.h"
#include "import.h"
#include "utf8_vn.h"

#define WORD_HASH_SIZE 26


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

#endif