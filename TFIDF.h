#ifndef _TF_IDF_H_
#define _TF_IDF_H_

#include "structure_data.h"
#include "import.h"
#include "wordhash.h"
#include "sparse_matrix.h"

TF_IDF_OJ* createTF_IDF(data_frame* df);
int build_vocab(TF_IDF_OJ* tfidf,int ngram);
int compute_idf(TF_IDF_OJ* tfidf,int alpha);
sparse_matrix* compute_tf_idf(sparse_matrix* tf_matrix,float* idf_vector,int size_vector);
sparse_matrix* compute_tf(TF_IDF_OJ* tfidf,data_frame* df,int alpha,int ngram);
sparse_matrix* fit_transform(TF_IDF_OJ* tfidf,int ngram);
sparse_matrix* transform(TF_IDF_OJ* original,data_frame* df,int ngram);
void freeTF_IDF(TF_IDF_OJ* tfidf);
void printTF_IDF(TF_IDF_OJ* tfidf);

#endif