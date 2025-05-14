#ifndef _DATAFRAME_H_
#define _DATAFRAME_H_

#include "import.h"
#include "structure_data.h"
#include "wordhash.h"
#include "string_pool.h"

extern char* label[]; // label name

char* getLabel(int index);
int countword(const char* str,char delim);
void parseVocab(const char* str,int n,char* delim,char** result);
int countLine(char* filename);
int dynamic_parse_string(FILE* file,data_frame* df,char* file_stopword);
int readFiletoDataFrame(FILE* file, data_frame* data_frame,int line,char* file_stopword);
void showDataFrame(data_frame* df);
void freeDataFrame(data_frame* df);
data_frame* createDataFrame();
char* remove_stop_word(char* str, char** stopword,int size);
char** load_stop_word(char* filename,int *save_size);
int remove_punctuation(char* str);
int remove_word_in_data_frame(data_frame* df, char** word,int size_word);
int writeDataFrameToFile(data_frame* df, const char* filename);
int remove_noise_in_dataframe(data_frame* df, word_hash* hash, int freq_threshold,char* file_stopword);

#endif