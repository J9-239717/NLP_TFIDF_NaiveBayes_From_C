#ifndef _IMPORT_H_
#define _IMPORT_H_


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define max(a,b) ((a) > (b) ? (a) : (b))

#define STOPWORDFILE "assets/stopword.txt"
//#define STOPWORDFILE "D:\\TestC_AI\\NLP_TFIDF_NaiveBayes_From_C\\assets\\stopword.txt"
#define test(a,b) do{\
    if(a != b){\
        fprintf(stderr, "Test failed at line %d\n", __LINE__);\
        return -1;\
    }else{\
        fprintf(stdout, "Test pass\n");\
    }\
}while(0);\

#endif