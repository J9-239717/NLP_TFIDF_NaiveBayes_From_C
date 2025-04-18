#include "h_files/dataframe.h"
#include "h_files/TFIDF.h"
#include "h_files/naive_bayes.h"

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

int write_test_pre_answer_to_file(data_frame* df, char* file){
    FILE* f = fopen(file,"w");
    if(!f){
        fprintf(stderr, "Cannot open file %s\n",file);
        return -1;
    }
    for(int i = 0; i < df->size; i++){
        fprintf(f,"%s ",df->data[i].label);
    }
    fclose(f);
    return 0;
}

int main(int argc, char* argv[]){
    if(argc != 5){
        printf("Usage: %s <file-Trian> <file-test-set> <file-real-test> <file-out-put-predict>\n",argv[0]);
        return 1;
    }
    char* filetrian = argv[1];
    char* filetest = argv[2];
    char* filerealout = argv[3];
    char* fileout = argv[4];

    // data frame trian set
    info_printf("Data Frame Train Set Create\n");
    start_timer();
    FILE* file_train = fopen(filetrian, "r");
    if(file_train == NULL){
        printf("Cannot open file %s\n", filetrian);
        return 1;
    }
    int line_train = countLine(filetrian);
    data_frame* df_train = createDataFrame();
    readFiletoDataFrame(file_train,df_train,line_train);
    fclose(file_train);
    show_time();

    // data frame test set
    info_printf("Data Frame Test Set Create\n");
    start_timer();
    FILE* file_test = fopen(filetest, "r");
    if(file_test == NULL){
        printf("Cannot open file %s\n", filetest);
        return 1;
    }
    int line_test = countLine(filetest);
    data_frame* df_test = createDataFrame();
    readFiletoDataFrame(file_test,df_test,line_test);
    write_test_pre_answer_to_file(df_test, filerealout);
    fclose(file_test);
    show_time();

    // get noise and delete noise in data frame
    info_printf("Get Noise and Delete Noise\n");
    start_timer();
    word_hash* noise = createWordHash();
    road_word_hash(noise, NOISEFILE);
    char** noise_word = NULL;
    int size_noise = 0;
    noise_word = getWordFormHash(noise, &size_noise);  
    freeWordHash(noise);
    // remove noise word in data frame
    remove_word_in_data_frame(df_train, noise_word, size_noise);
    remove_word_in_data_frame(df_test,noise_word,size_noise);
    freeArrayString(noise_word, size_noise);
    show_time();
    
    // tfidf of train and test
    info_printf("TFIDF Train And Transform\n");
    start_timer();
    TF_IDF_OJ* tfidf_train = createTF_IDF(df_train);
    int ngram = 2;
    fit_tfidf(tfidf_train, ngram);
    TF_IDF_OJ* tfidf_test = transform(tfidf_train,df_test,ngram);
    show_time();

    // ## TODO: should smote_oversample before fit model

    // fit model and predict
    info_printf("Naive Bayes Train and Predict\n");
    start_timer();
    Naive_Bayes_OJ* nb = createNaive_Bayes(df_train, tfidf_train->hash);
    fitNB(nb, tfidf_train, df_train);
    predict(nb, tfidf_test,fileout);
    show_time();

    // make null for not double free
    tfidf_test->hash = NULL;
    tfidf_test->idf_vector = NULL;

    // free memory
    info_printf("Clean Allocate\n");
    freeDataFrame(df_train);
    freeDataFrame(df_test);
    freeTF_IDF(tfidf_train);
    freeTF_IDF(tfidf_test);
    freeNaive_Bayes(nb);
    return 0;
}