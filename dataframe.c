#include "dataframe.h"

char* label[] = {
    "negative",
    "neutral",
    "positive"
};

// linear search stop word
int is_stop_word(char* str, char** stopword,int size){
    if(!str || !stopword) return 0;
    for(int i = 0; i < size; i++){
        if(memcmp(str, stopword[i],strlen(stopword[i])) == 0){
            return 1;
        }
    }
    return 0;
}

int is_special_char(char* str){
    char *special_char[] = {"…", "–", "_"};
    int size = sizeof(special_char)/sizeof(special_char[0]);
    for(int i = 0; i < size; i++){
        if(memcmp(str, special_char[i],strlen(special_char[i])) == 0){
            return 1;
        }
    }
    return 0;
}

char* remove_stop_word(char* str, char** stopword,int size_stopword){
    if(!str || !stopword) return NULL;
    char *token,*saveptr;
    char* rt = (char*)malloc(sizeof(char) * (strlen(str) + 1));
    if(!rt){
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    rt[0] = '\0';
    token = strtok_r(str, " ", &saveptr);
    while(token != NULL){
        if(!is_stop_word(token, stopword, size_stopword) && !is_special_char(token)){
            strcat(rt, token);
            strcat(rt, " ");
        }
        token = strtok_r(NULL, " ", &saveptr);
    }

    if(strlen(rt) > 0){
        rt[strlen(rt) - 1] = '\0'; // Remove trailing space
    }

    return rt;
}

void free_stop_word(char** stopword,int size){
    if(!stopword) return;
    for(int i = 0; i < size; i++){
        free(stopword[i]);
    }
    free(stopword);
}

// laod stop word from file
char** load_stop_word(char* filename,int *save_size){
    FILE* file = fopen(filename, "r");
    if(!file){
        fprintf(stderr, "Cannot open file %s\n", filename);
        return NULL;
    }
    int size = 0;
    char buffer[1024];
    // get size of stop word
    fgets(buffer, 1024, file);
    buffer[strcspn(buffer, "\n\r")] = '\0';
    size = atoi(buffer);
    if(size <= 0){
        fprintf(stderr, "Invalid size of stop word\n");
        fclose(file);
        return NULL;
    }
    if(save_size) *save_size = size;
    char** stopword = (char**)malloc(sizeof(char*) * size);
    if(!stopword){
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    // read stop word
    for(int i = 0; i < size; i++){
        if(fgets(buffer, 1024, file) == NULL){
            fprintf(stderr, "Error reading stop word\n");
            free_stop_word(stopword, size);
            fclose(file);
            return NULL;
        }
        buffer[strcspn(buffer, "\n\r")] = '\0';
        stopword[i] = strdup(buffer);
    }
    fclose(file);
    return stopword;
}

int remove_punctuation(char* str){
    if (!str){
        fprintf(stderr, "Invalid string pointer maybe your string is NULL in remove_punctuation\n");
        return 0;
    }
    char *src,*dst;
    src = dst = str;
    while(*src){
        if(!ispunct(*src)){
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
    return 1;
}

// create data frame and init
data_frame* createDataFrame(){
    data_frame* df = (data_frame*)malloc(sizeof(data_frame));
    if(!df){
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    df->size = 0;
    df->sizeKeys = 0;
    df->label_freq = NULL;
    df->keys = NULL;
    df->data = NULL;
    return df;
}

// create label frequency
label_frequency* createLabelFrequency(char* label){
    label_frequency* lf = (label_frequency*)malloc(sizeof(label_frequency));
    if(!lf){
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    lf->label = strdup(label);
    lf->frequency = 1;
    lf->next = NULL;
    return lf;
}

// add label frequency
int addLabelFrequency(data_frame* df, char* label){
    if(!df || !label){
        fprintf(stderr, "Invalid data frame or label\n");
        return 0;
    }
    // check if label already exists
    label_frequency* lf = df->label_freq;
    while(lf){
        if(memcmp(lf->label, label,strlen(label)) == 0){
            lf->frequency++;
            return 1;
        }
        lf = lf->next;
    }
    // create new label frequency
    label_frequency* new_lf = createLabelFrequency(label);
    if(!new_lf){
        fprintf(stderr, "Memory allocation failed\n");
        return 0;
    }
    new_lf->next = df->label_freq;
    df->label_freq = new_lf;
}

// show label and frequency
void showLabelFrequency(data_frame* df){
    if(!df || !df->label_freq){
        fprintf(stderr, "Invalid data frame or label frequency\n");
        return;
    }
    label_frequency* lf = df->label_freq;
    while(lf){
        printf("%s: %d\n", label[atoi(lf->label)], lf->frequency);
        lf = lf->next;
    }
}

// countword in string between delim, space and stop word
int countword(const char* str,char delim){
    int count = 0;
    int state = 0;
    while(*str){
        if(*str == ' ' || *str == delim || *str == '\0'){
            state = 0;
        }else if(state == 0){
            state = 1;
            count++;
        }
        str++;
    }
    return count;
}

// parse vocab in string between delim and store to result(array of string)
void parseVocab(const char* str,int n,char* delim,char** result){
    char *saveptr = NULL,*temp;
    // strtok_r is safe thread version
    temp = strtok_r((char*)str,delim,&saveptr);
    int i = 0;
    while(temp != NULL){
        result[i] = strdup(temp);
        temp = strtok_r(NULL,delim,&saveptr);
        i++;
    }
    printf("\n");
}

// countline in file
int countLine(char* filename){
    FILE* file = fopen(filename, "r");
    int count = 0;
    size_t size = 1024 * 1024;
    char buffer[size];
    while(fgets(buffer, size, file)){
        count++;
    }
    fclose(file);
    return count;
}

// parse sup string in string
char* parseString(char* str, char* delim,char** ptr) {
    int count = 0;
    char* c = str;
    *ptr = str;
    while(*c){
        if(*c == '"'){
            // get all string in ""
            c++;
            while(*c != '"'){
                c++;
            }
            *ptr = c+1;
            *c = '\0';
            return str+1;
        }

        if(*c == delim[0]){
            // pass delim and get string
            if(c == NULL){
                fprintf(stderr, "Error parsing string\n");
                return NULL;
            }
            *ptr = c+1;
            *c = '\0';
            return str;
        }
        c++;
    }
    return str;
}

// parse at end to start string stop at delim and make 
char* parseStringEnd(char* str, char* delim) {
    char* rt = NULL;
    size_t start_size = 1024;
    rt = (char*)malloc(sizeof(char) * start_size);
    if(!rt){
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    int length = strlen(str);
    char *c = str + length - 1;
    int i = 0;
    while(length > 0){
        if(*c == delim[0]){
            *c = '\0';
            rt[i] = '\0';
            return rt;
        }
        if(length == start_size - 1){
            start_size *= 2;
            char* temp = (char*)realloc(rt, sizeof(char) * start_size);
            if(!temp){
                fprintf(stderr, "Memory allocation failed\n");
                free(rt);
                return NULL;
            }
            rt = temp;
        }
        rt[i++] = *c;
        length--;
        c--;
    }
}

// remove c in string
void removeChar(char* str, char c){
    char* write_ptr = str;
    while(*str){
        if(*str != c){
            *write_ptr++ = *str;
        }
        str++;
    }
    *write_ptr = '\0';
}

// dynamic parse string to data_frame
// warning: should allocate array string first
// return max size of string in file
int dynamic_parse_string(FILE* file, data_frame* df) {
    int start_size = 1024;
    char* buffer = (char*)malloc(sizeof(char) * start_size);
    if (!buffer){
        fprintf(stderr,"Memory allocation failed\n");
        return -1;
    }
    int size_stopword = 0;
    char **stopword = load_stop_word(STOPWORDFILE, &size_stopword);
    if (!stopword) {
        fprintf(stderr, "Error loading stop words\n");
        free(buffer);
        return -1;
    }
    int max_size = 0, count = 0;
    char c;
    while ((c = fgetc(file)) != EOF) {

        // process 1 line
        if (c == '\n') {
            buffer[count] = '\0';
            max_size = (count > max_size) ? count : max_size;
            char* ptr = NULL;
            // parse label first
            df->data[df->size].label = parseStringEnd(buffer, ",");
            if(!addLabelFrequency(df, df->data[df->size].label)){
                fprintf(stderr, "Error adding label frequency\n");
                free(buffer);
                return -1;
            }

            // parse text
            if(!remove_punctuation(buffer)){
                fprintf(stderr, "Error removing punctuation\n");
                free(buffer);
                return -1;
            }
            char* text = remove_stop_word(buffer, stopword, size_stopword);
            df->data[df->size].text = text;
            df->data[df->size].count_word = 0;
            df->size++;
            free(buffer);
            buffer = (char*)malloc(sizeof(char) * start_size);
            count = 0;
            continue;
        }

        // re-size for store more
        if (count == start_size - 1) {
            start_size *= 2;
            char* temp = (char*)realloc(buffer, sizeof(char) * start_size);
            if(!temp){
                fprintf(stderr, "Memory allocation failed\n");
                free(buffer);
                return -1;
            }
            buffer = temp;
        }
        buffer[count++] = c;
    }

    // Handle the last line
    buffer[count] = '\0';
    max_size = (count > max_size) ? count : max_size;
    char* ptr = NULL;
    char* text = parseString(buffer, ",", &ptr);
    removeChar(ptr, ',');
    char* label = parseString(ptr, ",", &ptr);
    df->data[df->size].text = strdup(text);
    df->data[df->size].label = strdup(label);
    df->size++;
    

    free(buffer);
    free_stop_word(stopword, size_stopword);
    return max_size;
}

// main function of data frame
int readFiletoDataFrame(FILE* file, data_frame* data_frame,int line){
    char buffer[1024];

    if(!file){
        fprintf(stderr, "Invalid file pointer\n");
        return -1;
    }
    // read header
    if(!fgets(buffer, 1024, file)){
        fprintf(stderr, "Unable to read file header\n");
        return -1;
    }

    // init data_frame
    buffer[strcspn(buffer,"\n\r")] = '\0';
    int n = countword(buffer,',');
    data_frame->sizeKeys = n;
    data_frame->keys = (char**)malloc(sizeof(char*)*n);
    data_frame->data = (_data_frame_node*)malloc(sizeof(_data_frame_node)*line);

    if (!data_frame->keys || !data_frame->data) {
        fprintf(stderr, "Memory allocation failed\n");
        free(data_frame->keys);
        free(data_frame->data);
        return -1;
    }

    // parse header
    parseVocab(buffer,n,",",data_frame->keys);

    // parse data and label
    data_frame->size = 0;
    dynamic_parse_string(file,data_frame);
    return 0;
}

// show data frame
void showDataFrame(data_frame* df){
    for(int i = 0; i < df->sizeKeys; i++){
        printf("%s ",df->keys[i]);
    }
    printf("\n");

    for(int i = 0; i < df->size-1; i++){
        printf(">%s<\n",df->data[i].text);
        printf("label: %s and Count: %d\n",df->data[i].label,df->data[i].count_word);
    }

    printf("\n");
    // show label frequency
    showLabelFrequency(df);
}

// free allcate
void freeDataFrame(data_frame* df){
    if (df == NULL) return;

    // Free keys
    if (df->keys) {
        for(int i = 0; i < df->sizeKeys; i++) {
            free(df->keys[i]);
        }
        free(df->keys);
    }

    // Free data entries
    if (df->data) {
        for(int i = 0; i < df->size; i++) {
            free(df->data[i].text);
            free(df->data[i].label);
        }
        free(df->data);
    }

    // Free label frequency
    label_frequency* lf = df->label_freq;
    while(lf){
        label_frequency* temp = lf;
        lf = lf->next;
        free(temp->label);
        free(temp);
    }

    free(df);
}