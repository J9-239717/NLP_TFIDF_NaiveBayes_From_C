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

// Matrix
//  -------- -------- --------  
// |   TP   |   FP   |   FP   |
//  -------- -------- --------
// |   FN   |   TN   |   TN   |
//  -------- -------- --------
// |  FNE   |  FNE   |   TNE  |
//  -------- -------- --------
// P = positive
// N = negative
// NE = neutral
// T = true
// F = false
enum { W_LABEL = 10, W_METRIC = 8, W_COUNT = 6 };
void calculate_accuracy(char* file_real, char* file_predict){
    FILE* f1 = fopen(file_real,"r");
    FILE* f2 = fopen(file_predict,"r");
    if(f1 == NULL || f2 == NULL){
        fprintf(stderr, "Cannot open file %s or %s\n",file_real,file_predict);
        return;
    }
    // matrix_ture_and_predict
    int mtx[3][3] = {0};
    size_t size1 = 0,size2 = 0;

    fseek(f1, 0, SEEK_END);
    size1 = ftell(f1);
    fseek(f1, 0, SEEK_SET);

    fseek(f2, 0, SEEK_END);
    size2 = ftell(f2);
    fseek(f2, 0, SEEK_SET);

    if(size1 != size2){
        fprintf(stderr, "The size of file %s and %s is not equal\n",file_real,file_predict);
        fclose(f1);
        fclose(f2);
        return;
    }

    char* line1 = malloc(size1 + 1);
    char* line2 = malloc(size2 + 1);
    checkExistMemory(line1);
    checkExistMemory(line2);

    size_t bytes_read1 = fread(line1, 1, size1, f1);
    size_t bytes_read2 = fread(line2, 1, size2, f2);

    if (bytes_read1 != size1) {
        error_printf("Error reading from file %s (read %zu/%zu bytes)\n", file_real, bytes_read1, size1);
    }

    if (bytes_read2 != size2) {
        error_printf("Error reading from file %s (read %zu/%zu bytes)\n", file_predict, bytes_read2, size2);
    }

    fclose(f1);
    fclose(f2);

    line1[size1] = '\0';
    line2[size2] = '\0';

    int real = 0;
    int predict = 0;
    for(int i = 0; i < size1; i++){
        if(line1[i] == ' ' || line2[i] == ' '){
            continue;
        }
        real = line1[i] - '0';
        predict = line2[i] - '0';
        mtx[real][predict]++;
    }
    free(line1);
    free(line2);

    int Total = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            Total += mtx[i][j];
        }
    }
    

    char *label[] = {"negative", "neutral", "positive"};
    int Row_Total[3] = {0};
    int Column_Total[3] = {0};
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(i != j){
                Row_Total[i] += mtx[i][j];
                Column_Total[j] += mtx[i][j];
            }
        }
    }
    // overall accuracy
    float accuracy_all = 
        (mtx[0][0] + mtx[1][1] + mtx[2][2]) / (float)Total;
    printf("Overall accuracy: %.2f%%\n\n", accuracy_all * 100);

    // header
    printf("%-*s | %*s | %*s | %*s | %*s | %*s\n",
           W_LABEL, "Label",
           W_METRIC+1, "Acc",
           W_METRIC+1, "Prec",
           W_METRIC+1, "Rec",
           W_METRIC+1, "F1",
           W_COUNT+3,  "Count");

    // separator
    printf("%.*s-+-%.*s-+-%.*s-+-%.*s-+-%.*s-+-%.*s\n",
           W_LABEL,  "----------",
           W_METRIC+1, "---------",
           W_METRIC+1, "---------",
           W_METRIC+1, "---------",
           W_METRIC+1, "---------",
           W_COUNT+3,  "-----------");

    // rows
    for(int i = 0; i < 3; i++){
        float acc  = mtx[i][i] / (float)(mtx[i][i] + Row_Total[i] + Column_Total[i]);
        float prec = mtx[i][i] / (float)(mtx[i][i] + Column_Total[i]);
        float rec  = mtx[i][i] / (float)(mtx[i][i] + Row_Total[i]);
        float f1   = 2 * (prec * rec) / (prec + rec);
        int   cnt  = mtx[i][0] + mtx[i][1] + mtx[i][2];

        printf("%-*s | %*.2f%% | %*.2f%% | %*.2f%% | %*.2f%% | %*d\n",
               W_LABEL,  label[i],
               W_METRIC, acc  * 100,
               W_METRIC, prec * 100,
               W_METRIC, rec  * 100,
               W_METRIC, f1   * 100,
               W_COUNT,  cnt);
    }
    printf("\n\n");
    return;
}

int main(int argc, char* argv[]){
    if(argc != 9){
        printf("Usage: %s <file-Train> <file-test-set> <file-real-test> <file-out-put-predict> <file-stop-word> <file-noise> <num-of-ngram> <op>\n",argv[0]);
        printf("for op = c_n is get noise word from freq <= n; n is should Number and write to file in directory run this program "
                "   op = null or other for run normally \n");
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
    if(isdigit(argv[7][0]) == 0){
        error_printf("The ngram must be a number\n");
        return 1;
    }
    int ngram = atoi(argv[7]);
    if(argv[8][0] == 'c' && argv[8][1] == '_'){
        if(isdigit(argv[8][2]) == 0){
            error_printf("c_n: n is should a number\n");
            return 1;
        }
        goto getNoise;
    }

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
        info_printf("The noise word file is either not a .txt file or is invalid. Noise processing will be skipped.\n");
        fileNoise = NULL;
    }
    
    pthread_t thread1, thread2, thread3;

    // create thread to read data frame
    thread_df *data_train;
    data_train = malloc(sizeof(thread_df));
    checkExistMemory(data_train);
    data_train->file_name = filetrian;
    data_train->stopword = fileStopWord;
    data_train->df = NULL;
    data_train->line = 0;

    if(pthread_create(&thread1, NULL, init_data_frame, (void*)data_train) != 0){
        fprintf(stderr, "Error creating thread 1\n");
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
    free(data_train);

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
        freeArrayString(noise_word, size_noise);
        show_time();
    }
    // tfidf of train and test
    info_printf("TFIDF Train And Transform\n");
    start_timer();
    TF_IDF_OJ* tfidf_train = createTF_IDF(df_train);
    info_printf("TFIDF ngram = %d\n",ngram);
    fit_tfidf(tfidf_train, ngram);
    show_time();

    // fit model and predict
    info_printf("Naive Bayes Train and Predict\n");
    start_timer();
    Naive_Bayes_OJ* nb = createNaive_Bayes(df_train, tfidf_train->hash);
    fitNB(nb, tfidf_train, df_train);
    show_time();

    while(1){
        char buffer_test[256];
        data_frame* df_test = NULL;
        TF_IDF_OJ* tfidf_test = NULL;
        printf("Enter the test sentence : ");
        fgets(buffer_test, sizeof(buffer_test), stdin);
        buffer_test[strcspn(buffer_test, "\n")] = '\0';
        if(buffer_test[0] == ':' && buffer_test[1] == 'q'){ 
            break;
        }
        char headder[] = "text,label\n";
        FILE* f = fopen("temp.csv", "w");
        if(f == NULL){
            fprintf(stderr, "Cannot open file temp.csv\n");
            return 1;
        }
        fprintf(f, "%s", headder);
        fprintf(f, "%s,-1", buffer_test);
        fclose(f);
        f = fopen("temp.csv", "r");
        if(f == NULL){
            fprintf(stderr, "Cannot open file temp.csv\n");
            return 1;
        }
        int line_count = countLine("temp.csv");
        df_test = createDataFrame();
        readFiletoDataFrame(f, df_test, line_count, fileStopWord);
        fclose(f);
        tfidf_test = transform(tfidf_train, df_test, ngram);
        predict(nb, tfidf_test, fileout, "null");

        // make null for not double free
        tfidf_test->hash = NULL;
        tfidf_test->idf_vector = NULL;
        
        // free reset
        if(tfidf_test != NULL){
            freeTF_IDF(tfidf_test);
        }
        if(df_test != NULL){
            freeDataFrame(df_test);
        }
    
    }

    // free memory
    info_printf("Clean Allocate\n");
    freeDataFrame(df_train);
    freeTF_IDF(tfidf_train);
    freeNaive_Bayes(nb);
    if(0){
        getNoise:
        info_printf("Get Noise Word from low frequency\n");
        int n = atoi(argv[8]+2);
        FILE* file = fopen(filetrian,"r");
        if(file == NULL){
            fprintf(stderr, "Cannot open file %s\n", filetrian);
            return 1;
        }
        int line_count = countLine(filetrian);
        data_frame* df = createDataFrame();
        readFiletoDataFrame(file,df,line_count,fileStopWord);
        word_hash* hash = WordHashWithNgram_MultiThread(df, 1);
        word_hash* noise = smooth_word(hash, n);
        if(noise == NULL){
            fprintf(stderr, "Cannot get noise word\n");
            freeDataFrame(df);
            freeWordHash(hash);
            return 1;
        }
        writeWordHashToFile(noise, "noise.txt");
        freeWordHash(noise);
        freeWordHash(hash);
        freeDataFrame(df);
        fclose(file);
    }
    return 0;
}