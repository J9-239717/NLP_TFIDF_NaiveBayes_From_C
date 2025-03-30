#include "dataframe.h"
#include "wordhash.h"

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
    word_hash* hash = WordHash(df);
    printf("Word Hash Table Size %d:\n", hash->size);
    word_hash* noise = smooth_word(hash,3);
    printf("Word Hash Table Size %d:\n", hash->size);
    writeWordHashToFile(hash, "assets/wordhash.txt");
    printf("Word Noise Size %d:\n", noise->size);
    writeWordHashToFile(noise, "assets/wordnoise.txt");
    freeDataFrame(df);
    freeWordHash(hash);
    freeWordHash(noise);
    fclose(file);
    return 0;
}