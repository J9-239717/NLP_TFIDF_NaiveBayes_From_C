#include "dataframe.h"
#include "TFIDF.h"

void freeArrayString(char** src,int size){
    if(!src) return;
    for(int i = 0; i < size; i++){
        free(src[i]);
    }
    free(src);
}

void log_string(char**src,int size,char*file){
    FILE* f = fopen(file,"w");
    if(!f){
        fprintf(stderr, "Cannot open file %s\n",file);
        return;
    }
    for(int i = 0; i < size; i++){
        fprintf(f,"<%s>\n",src[i]);
    }
    fclose(f);
}

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Usage: %s <filename>\n",argv[0]);
        return 1;
    }
    char* filename = argv[1];
    FILE* file = fopen(filename, "r");
    if(file == NULL){
        printf("Cannot open file %s\n",filename);
        return 1;
    }
    int line = countLine(filename);
    // create data frame
    printf("Create data frame phase\n");
    start_timer();
    data_frame* df = createDataFrame();
    readFiletoDataFrame(file,df,line);
    show_time();
    test(df->size, line-1);
    fclose(file);

    // get noise and delete noise in data frame
    printf("Get noise phase\n");
    start_timer();
    word_hash* noise = createWordHash();
    road_word_hash(noise, NOISEFILE);
    char** noise_word = NULL;
    int size_noise = 0;
    noise_word = getWordFormHash(noise, &size_noise);
    show_time();    
    freeWordHash(noise);

    // remove noise word in data frame
    printf("Remove Noise phase\n");
    start_timer();
    remove_word_in_data_frame(df, noise_word, size_noise);
    freeArrayString(noise_word, size_noise);
    show_time();
    // writeDataFrameToFile(df, "assets/debug.txt");
    // get vocab to make feature engineering
    //word_hash* hash = WordHash(df);
    //writeWordHashToFile(hash, "assets/vocab.txt");

    // N-gram to make feature engineering
    // printf("N-Gram phase\n");
    // word_hash* ngram = WordHashWithNgram(df, 2);
    // char** ngram_word = NULL;
    // int size_ngram = 0;
    // ngram_word = getWordFormHash(ngram, &size_ngram);
    
    // ## TODO: Try count IDF for filter noise word

    // ## TODO: make spares matrix
    
    // ## TODO: make TF-IDF
    printf("TF-IDF phase\n");
    TF_IDF_OJ* tfidf = createTF_IDF(df);
    int ngram = 2;
    sparse_matrix*temp = fit_transform(tfidf, ngram);
    // test(temp->rows, df->size);
    // test(temp->cols, tfidf->hash->size);
    //printTF_IDF(tfidf);
    // ## TODO: make Naive Bayes
    // ## TODO: try predict
    // ## TODO: review accuracy
    printf("Free phase\n");
    freeDataFrame(df);
    freeTF_IDF(tfidf);
    // freeWordHash(ngram);
    // freeArrayString(ngram_word, size_ngram);
    return 0;
}