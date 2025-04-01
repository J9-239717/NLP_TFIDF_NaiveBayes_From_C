#ifndef _DATAFRAME_H_
#define _DATAFRAME_H_

#include "import.h"

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

extern char* label[]; // label name

int countword(const char* str,char delim);
void parseVocab(const char* str,int n,char* delim,char** result);
int countLine(char* filename);
int dynamic_parse_string(FILE* file,data_frame* df);
int readFiletoDataFrame(FILE* file, data_frame* data_frame,int line);
void showDataFrame(data_frame* df);
void freeDataFrame(data_frame* df);
data_frame* createDataFrame();
char* remove_stop_word(char* str, char** stopword,int size);
char** load_stop_word(char* filename,int *save_size);
int remove_punctuation(char* str);
int remove_word_in_data_frame(data_frame* df, char** word,int size_word);
int writeDataFrameToFile(data_frame* df, const char* filename);

#endif