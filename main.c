#include "dataframe.h"
#include "wordhash.h"

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
    data_frame* df = createDataFrame();
    readFiletoDataFrame(file,df,line);
    writeDataFrameToFile(df, "assets/logframe_origin.txt");
    printf("<%s>\n", df->data[0].label);
    word_hash* hash = WordHash(df);
    // writeDataFrameToFile(df, "assets/logframe2.txt");
    printf("Word Hash Table Size %d:\n", hash->size);
    word_hash* noise = smooth_word(hash,3);
    printf("Word Hash Table Size %d:\n", hash->size);
    // writeWordHashToFile(hash, "assets/wordhash.txt");
    printf("Word Noise Size %d:\n", noise->size);
    // writeWordHashToFile(noise, "assets/wordnoise.txt");
    int size_noise = 0;
    char** noise_w = getWordFormHash(noise,&size_noise);
    //writeDataFrameToFile(df, "assets/logframe1.txt");
    // ## TODO: delete word noise in data frame
    // ## BUG: fix remove word in data frame
    remove_word_in_data_frame(df, noise_w,size_noise);
    //log_string(noise_w, noise->size, "assets/logstring.txt");
    writeDataFrameToFile(df, "assets/logframe.txt");
    // ## TODO: Try count IDF for filter noise word
    // ## TODO: make N-gram
    // ## TODO: add word N-gram to hash table
    // ## TODO: make spares matrix
    // ## TODO: make TF-IDF
    // ## TODO: make Naive Bayes
    // ## TODO: try predict
    // ## TODO: review accuracy
    freeArrayString(noise_w, noise->size);
    freeDataFrame(df);
    freeWordHash(hash);
    freeWordHash(noise);
    fclose(file);
    return 0;
}