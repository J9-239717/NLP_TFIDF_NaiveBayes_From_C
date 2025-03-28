#include "dataframe.h"

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
    test(line,df->size);
    showDataFrame(df);
    freeDataFrame(df);
    fclose(file);
    return 0;
}