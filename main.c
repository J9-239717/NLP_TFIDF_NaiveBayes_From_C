#include "h_files/dataframe.h"
#include "h_files/TFIDF.h"
#include "h_files/naive_bayes.h"

typedef struct _thread_df{
    char* file_name;
    char* stopword;
    data_frame* df;
    int line;
}thread_df;

typedef struct _thread_word_hash{
    char* file_name;
    word_hash* hash;
    char** word;
    int size_word;
}thread_word_hash;

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

void* init_data_frame(void* arg){
    thread_df* data = (thread_df*)arg;
    FILE* file = fopen(data->file_name,"r");
    if(file == NULL){
        fprintf(stderr, "Cannot open file %s\n",data->file_name);
        return NULL;
    }
    int line = countLine(data->file_name);
    data_frame* df = createDataFrame();
    readFiletoDataFrame(file,df,line,data->stopword);
    fclose(file);
    data->df = df;
    return NULL;
}

void* get_noise(void* arg){
    thread_word_hash* data = (thread_word_hash*)arg;
    word_hash* hash = createWordHash();
    road_word_hash(hash, data->file_name);
    char** noise_word = NULL;
    int size_noise = 0;
    noise_word = getWordFormHash(hash, &size_noise);  
    freeWordHash(hash);
    data->word = noise_word;
    data->size_word = size_noise;
    return NULL;
}

int main(int argc, char* argv[]){
    if(argc != 7){
        printf("Usage: %s <file-Trian> <file-test-set> <file-real-test> <file-out-put-predict> <file-stop-word> <file-noise> \n",argv[0]);
        return 1;
    }
    info_printf("create data frame and noise word\n");
    start_timer();
    char* filetrian = argv[1];
    char* filetest = argv[2];
    char* filerealout = argv[3];
    char* fileout = argv[4];
    char* fileStopWord = argv[5];
    char* fileNoise = argv[6];

    // check noise word
    if(strstr(fileNoise,".txt") != NULL){
        FILE* f = fopen(fileNoise,"r");
        if(f == NULL){
            info_printf("Cannot open file or Don't have file noise : %s\n",fileNoise);
            fileNoise = NULL;
        }else{
            fclose(f);
        }
    }else{
        info_printf("File noise word is not .txt file or something invalide ( program will cancel noise ) \n");
        fileNoise = NULL;
    }
    
    pthread_t thread1, thread2, thread3;

    // create thread to read data frame
    thread_df *data_train,*data_test; 
    data_train = malloc(sizeof(thread_df));
    data_test = malloc(sizeof(thread_df));
    checkExistMemory(data_train);
    checkExistMemory(data_test);
    data_train->file_name = filetrian;
    data_test->file_name = filetest;
    data_train->stopword = data_test->stopword = fileStopWord;
    data_train->df = data_test->df = NULL;
    data_train->line = data_test->line = 0;

    if(pthread_create(&thread1, NULL, init_data_frame, (void*)data_train) != 0){
        fprintf(stderr, "Error creating thread 1\n");
        return 1;
    }
    if(pthread_create(&thread2, NULL, init_data_frame, (void*)data_test) != 0){
        fprintf(stderr, "Error creating thread 2\n");
        return 1;
    }

    // create thread to read noise word
    thread_word_hash *data_noise;
    if(fileNoise != NULL){
        data_noise = malloc(sizeof(thread_word_hash));
        checkExistMemory(data_noise);
        data_noise->file_name = fileNoise;
        data_noise->hash = NULL;
        data_noise->word = NULL;
        data_noise->size_word = 0;
    }

    if(fileNoise != NULL){
        if(pthread_create(&thread3, NULL, get_noise, (void*)data_noise) != 0){
            fprintf(stderr, "Error creating thread 3\n");
            return 1;
        }
    }
    
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    if(fileNoise != NULL){
        pthread_join(thread3, NULL);
    }
    show_time();

    data_frame* df_train = data_train->df;
    data_frame* df_test = data_test->df;
    write_test_pre_answer_to_file(df_test, filerealout);
    free(data_train);
    free(data_test);

    if(fileNoise != NULL){
        // remove noise word in data frame
        info_printf("remove word in data frame\n");
        start_timer();
        word_hash* noise_hash = data_noise->hash;
        char** noise_word = data_noise->word;
        int size_noise = data_noise->size_word;
        free(data_noise);
        freeWordHash(noise_hash);
        remove_word_in_data_frame(df_train, noise_word, size_noise);
        remove_word_in_data_frame(df_test,noise_word,size_noise);
        freeArrayString(noise_word, size_noise);
        show_time();
    }
    // tfidf of train and test
    info_printf("TFIDF Train And Transform\n");
    start_timer();
    TF_IDF_OJ* tfidf_train = createTF_IDF(df_train);
    int ngram = 2;
    info_printf("TFIDF ngram = %d\n",ngram);
    fit_tfidf(tfidf_train, ngram);
    TF_IDF_OJ* tfidf_test = transform(tfidf_train,df_test,ngram);
    show_time();

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