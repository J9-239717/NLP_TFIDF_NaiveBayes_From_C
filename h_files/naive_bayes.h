#ifndef _NAIVE_BAYES_H_
#define _NAIVE_BAYES_H_

#include "structure_data.h"
#include "import.h"
#include "wordhash.h"
#include "sparse_matrix.h"


Naive_Bayes_OJ* createNaive_Bayes(data_frame* df, word_hash* hash);
void freeNaive_Bayes(Naive_Bayes_OJ* nb);
void printNaive_Bayes(Naive_Bayes_OJ* nb);
int fitNB(Naive_Bayes_OJ* nb, TF_IDF_OJ* tf_idf, data_frame* df);
int getlikelihood_to_file(Naive_Bayes_OJ* nb, const char* filename);
int predict(Naive_Bayes_OJ* model, TF_IDF_OJ* tf_idf,char* file_predict);

#endif