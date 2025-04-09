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
        if(memcmp(str, stopword[i],strlen(stopword[i])) == 0 && strlen(stopword[i]) == strlen(str)){
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
    char* rt = (char*)malloc(sizeof(char) * (strlen(str) + 2));
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
            buffer[strcspn(buffer, "\n\r")] = '\0';
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
            df->data[df->size].count_word = countword(text, ' ');
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
    if(!remove_punctuation(text)){
        fprintf(stderr, "Error removing punctuation\n");
        free(buffer);
        return -1;
    }
    char* label = parseString(ptr, ",", &ptr);
    df->data[df->size].text = strdup(text);
    df->data[df->size].label = strdup(label);
    df->data[df->size].count_word = countword(text, ' ');
    df->size++;
    

    free(buffer);
    free_stop_word(stopword, size_stopword);
    return max_size;
}

// main function of data frame
// read file to data frame
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
        printf(">%s<",df->data[i].text);
    }

    printf("\n");
    // show label frequency
    showLabelFrequency(df);
}

// write data frame to file
int writeDataFrameToFile(data_frame* df, const char* filename){
    if(!df || !filename){
        fprintf(stderr, "Invalid data frame or filename\n");
        return -1;
    }
    FILE* file = fopen(filename, "w");
    if(!file){
        fprintf(stderr, "Cannot open file %s\n", filename);
        return -1;
    }
    for(int i = 0; i < df->sizeKeys; i++){
        fprintf(file, "%s <-> ", df->keys[i]);
    }
    fprintf(file, "\n");

    for(int i = 0; i < df->size; i++){
        fprintf(file, "< %s > is <%s> and count is %d\n", df->data[i].text, df->data[i].label, df->data[i].count_word);
    }
    fclose(file);
    return 0;
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

int is_token_in_array(char* token,char **array,int size){
    if(!token || !array){
        fprintf(stderr, "Invalid token or array\n");
        exit(EXIT_FAILURE);
    }
    size_t min_len = 3;
    for(int i = 0; i < size; i++){
        size_t len = strlen(array[i]);
        len = len < min_len ? min_len : len;
        if(memcmp(token, array[i],len) == 0){
            return 1;
        }
    }
    return 0;
}

void write_word(char* token,char* src,int* origin_index,int size){
    if(token == NULL || src == NULL || origin_index == NULL){
        fprintf(stderr, "Invalid token or source string\n");
        return;
    }
    for(int i = 0; i < size; i++){
        src[*origin_index] = token[i];
        (*origin_index)++;
    }
    src[*origin_index] = ' ';
    (*origin_index)++;
}

int remove_word_in_data_frame(data_frame* df, char** word,int size_word){
    if(!df || !word) {
        fprintf(stderr, "Invalid data frame or word\n");
        return 0;
    }
    // FILE* file = fopen("assets/debug.txt", "w");
    int min_len = 6;
    for(int i = 0; i < df->size; i++){
        char *temp = NULL,*token = NULL,*saveptr = NULL;
        int exit_flag = 0;
        char* text = strdup(df->data[i].text);
        char* save = text;
        if(!text){
            fprintf(stderr, "Invalid text in data frame\n");
            continue;
        }
        size_t len = strlen(df->data[i].text) * 2;
        len = len < min_len ? min_len : len;
        temp = (char*)calloc(len, sizeof(char));
        if(!temp){
            fprintf(stderr, "Memory allocation failed\n");
            return 0;
        }
        int origin_index = 0;
        token = strtok_r(text, " ", &saveptr);
        while(token != NULL){
            int flag = is_token_in_array(token, word,size_word);
            if(!flag){
                exit_flag = 1;
                write_word(token, temp, &origin_index, strlen(token));
                // fprintf(file, "(%s,%d)", temp,i);
            }
            token = strtok_r(NULL, " ", &saveptr);
        }
        if (exit_flag) {
            temp[origin_index] = '\0';
        }
        free(df->data[i].text);
        df->data[i].text = strdup(temp);
        df->data[i].count_word = countword(temp, ' ');
        free(temp);
        free(save);
        // fprintf(file, "\n");
    }
    // fclose(file);
    return 1;
}

int is_english_word(const char* word) {
    if (!word) return 0;

    while (*word) {
        if (!isalpha(*word)) {
            return 0; // Not an English word (has number, symbol, or Vietnamese char)
        }
        word++;
    }
    return 1; // All characters are English alphabet letters
}


int is_special_noise(char* str) {
    // Check if token contains digits or special characters
    while (*str) {
        if (isdigit(*str) || ispunct(*str)) return 1;
        if ((*str & 0x80)) return 0;  // Keep Vietnamese characters
        str++;
    }
    return 0;
}

int isNumber(char* str) {
    if (!str) return 0;
    while (*str) {
        if (isdigit(*str)) return 1;
        str++;
    }
    return 0;
}

int remove_noise_in_dataframe(data_frame* df, word_hash* hash, int freq_threshold){
    if (!df){
        fprintf(stderr, "Invalid data frame\n");
        return -1;
    }
    
    // flag check if hash is exist it's mean we will check in hash frequency
    // but it not exist we will pass
    // nflag = 1 mean we will not check in hash
    // nflag = 0 mean we will check in hash
    int nflag = 0;
    if (!hash) {
        nflag = 1;
    }

    int stopword_size = 0;
    char **stopwords = load_stop_word(STOPWORDFILE, &stopword_size);
    if (!stopwords) {
        fprintf(stderr, "Error loading stop words\n");
        return -1;
    }

    for (int i = 0; i < df->size; i++) {
        char *text = strdup(df->data[i].text);
        if (!text) continue;

        char buffer[2048] = "";
        char* token;
        char* saveptr = NULL;

        token = strtok_r(text, " ", &saveptr);
        while (token != NULL) {

            if(isNumber(token)){
                token = strtok_r(NULL, " ", &saveptr);
                continue;
            }

            int freq_check = 0;
            if(nflag){
                // if hash is not exist we will not check in hash
                // always pass
                freq_check = 1;
            }else{
                int freq = getWordFreq(hash, token);
                freq_check = freq > freq_threshold ? 1 : 0;
            }
            
            
            // Keep token if it meets these conditions
            if (
                freq_check && 
                !is_stop_word(token, stopwords, stopword_size) &&
                !is_special_noise(token)
            ) {
                strcat(buffer, token);
                strcat(buffer, " ");
            }
            token = strtok_r(NULL, " ", &saveptr);
        }

        // Update dataframe text
        buffer[strlen(buffer) - 1] = '\0';  // Remove trailing space
        free(df->data[i].text);
        df->data[i].text = strdup(buffer);
        free(text);
    }
    free_stop_word(stopwords, stopword_size);
    return 1;
}
