#include "dataframe.h"
#include "TFIDF.h"
#include "naive_bayes.h"

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
    func_printf("Create data frame phase");
    start_timer();
    data_frame* df = createDataFrame();
    readFiletoDataFrame(file,df,line);
    show_time();
    test(df->size, line-1);
    info_printf("Data frame size: %d\n", df->size);
    fclose(file);

    // get noise and delete noise in data frame
    func_printf("Get noise phase\n");
    start_timer();
    word_hash* noise = createWordHash();
    road_word_hash(noise, NOISEFILE);
    char** noise_word = NULL;
    int size_noise = 0;
    noise_word = getWordFormHash(noise, &size_noise);
    show_time();    
    freeWordHash(noise);

    // remove noise word in data frame
    func_printf("Remove Noise phase\n");
    start_timer();
    remove_word_in_data_frame(df, noise_word, size_noise);
    freeArrayString(noise_word, size_noise);
    show_time();
    
    func_printf("TF-IDF phase\n");
    TF_IDF_OJ* tfidf = createTF_IDF(df);
    int ngram = 2;
    sparse_matrix*temp = fit_transform(tfidf, ngram);
    test(temp->rows, df->size);
    test(temp->cols, tfidf->hash->size);
    test(tfidf->tf_idf_matrix->size, tfidf->hash->size);

    // ## TODO: should smote_oversample before fit model
    func_printf("Naive Bayes phase\n");
    Naive_Bayes_OJ* nb = createNaive_Bayes(df, tfidf->hash);
    fitNB(nb, tfidf, df);
    printNaive_Bayes(nb);
    getlikelihood_to_file(nb, "assets/likelihood.txt");
    // ## TODO: try predict
    func_printf("Predict phase\n");

    // ## TODO: review accuracy
    func_printf("Accuracy phase\n");

    // free memory
    func_printf("Free phase\n");
    freeDataFrame(df);
    freeTF_IDF(tfidf);
    freeNaive_Bayes(nb);
    return 0;
}