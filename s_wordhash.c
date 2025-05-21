#include "h_files/wordhash.h"

typedef struct _thread_pack_{
    int start;
    int end;
    word_hash* hash;
    data_frame* global_df;
    int ngram;
}thread_pack_word_hash;

word_node_pool* createWordNodePool() {
    word_node_pool* pool = malloc(sizeof(word_node_pool));
    checkExistMemory(pool);

    pool->pool = (word_node**)malloc(sizeof(word_node*) * BLOCK_SIZE_WORD_HASH);
    checkExistMemory(pool->pool);

    for(int i = 0; i < BLOCK_SIZE_WORD_HASH; i++) {
        pool->pool[i] = NULL;
    }

    pool->pool[0] = (word_node*) malloc(sizeof(word_node) * INITAL_WORD_HASH_SIZE_POOL);
    checkExistMemory(pool->pool[0]);

    pool->block_size = BLOCK_SIZE_WORD_HASH;
    pool->used_block = 0;  // index block
    pool->used = 0;        // used node in current block
    return pool;
}

int freeWordNodePool(word_node_pool* pool) {
    if (!pool) {
        fprintf(stderr, "Word node pool is NULL\n");
        return -1;
    }

    for (int i = 0; i <= pool->used_block; i++) {
        free(pool->pool[i]);
    }
    free(pool->pool);
    free(pool);
    return 0;
}


word_node* allocate_node(word_node_pool* pool) {
    checkExistMemory(pool);
    if (pool->used >= INITAL_WORD_HASH_SIZE_POOL) {
        pool->used_block++;
        if (pool->used_block >= pool->block_size) {
            pool->block_size *= 2;
            pool->pool = realloc(pool->pool, sizeof(word_node*) * pool->block_size);
            fprintf(stderr, "Reallocating memory for word node pool\n");
            checkExistMemory(pool->pool);
        }
        pool->pool[pool->used_block] = malloc(sizeof(word_node) * INITAL_WORD_HASH_SIZE_POOL);
        checkExistMemory(pool->pool[pool->used_block]);
        pool->used = 0;
    }
    word_node* node = &pool->pool[pool->used_block][pool->used++];
    node->word = NULL;
    node->freq = 0;
    node->next = NULL;
    return node;
}

void reset_word_node_pool(word_node_pool* pool) {
    if (!pool) {
        fprintf(stderr, "Word node pool is NULL\n");
        return;
    }
    // Not free the pool, just reset the used index
    for(int i = 1; i <= pool->used_block; i++) {
        if(pool->pool[i]) {
            free(pool->pool[i]);
            pool->pool[i] = NULL;
        }
    }
    pool->used = 0;
    pool->used_block = 0;

}

// This function is used to create a new word_node
// It will allocate memory for the new node and initialize it
word_node* createNodeWord(char* word,StringPool* pool, word_node_pool* node_pool){
    word_node* newNode = allocate_node(node_pool);
    checkExistMemory(newNode);
    newNode->word = str_pool_alloc(pool, word);
    newNode->freq = 1;
    return newNode;
}

// Create a new word_hash
word_hash* createWordHash(){
    word_hash* newHash = (word_hash*)malloc(sizeof(word_hash));
    if(!newHash){
        fprintf(stderr, "Memory allocation failed for newHash\n");
        return NULL;
    }
    newHash->size = 0;
    for(int i = 0; i < WORD_HASH_SIZE; i++){
        newHash->table[i] = NULL;
        newHash->size_each[i] = 0;
    }
    newHash->str_pool = create_string_pool();
    newHash->node_pool = createWordNodePool();
    return newHash;
}

// Reset the word hash
void resetWordHash(word_hash* hash){
    if(!hash){
        fprintf(stderr, "Hash table is NULL\n");
        return;
    }
    for(int i = 0; i < WORD_HASH_SIZE; i++){
        hash->size_each[i] = 0;
        hash->table[i] = NULL;
    }
    hash->size = 0;
    reset_string_pool(hash->str_pool);
    reset_word_node_pool(hash->node_pool);
}

// Convert first character of word to English
// Return the index for the hash table
inline __attribute__((always_inline)) char getIndexHashWord(char* word){
    if(!word){
        fprintf(stderr, "Word is NULL\n");
        return -1;
    }
    char index = 0;
    // key is first char
    char temp = covert_to_eng(word);
    if(temp == '\0'){
        error_printf("Invalid character in word: %s is \\0 in line: %d in file: %s\n", word, __LINE__, __FILE__);
        exit(EXIT_FAILURE);
    }

    // if is digit pass
    if(isdigit(temp)){
        error_printf("Invalid character in word: %s is digit in line: %d in file: %s\n", word, __LINE__, __FILE__);
        return -1; // Invalid index for digits
    }

    index = tolower(temp) - 'a';
    if(index < 0 || index >= WORD_HASH_SIZE){
        return INVALID_DATA_INDEX; // Invalid index
    }
    return index;
}

// Check if word exists in hash table
// Return the node if it exists, otherwise return NULL
word_node* isExistWordHash(word_hash* hash, char* word){
    if(!hash || !word){
        fprintf(stderr, "Hash table or word is NULL\n");
        return 0;
    }
    char index = getIndexHashWord(word);
    if(index == -1){
        fprintf(stderr, "Invalid index for word: %s in line:%d from file: %s\n", word, __LINE__, __FILE__);
        exit(EXIT_FAILURE); // Error index
    }

    word_node* current = hash->table[index];
    while(current){
        if(memcmp(current->word, word,strlen(word)) == 0 && strlen(current->word) == strlen(word)){
            return current; // Word exists
        }
        current = current->next;
    }
    return NULL; // Word does not exist
}

// Add a word to the hash table
// If the word already exists, increment its frequency
void push_word(word_hash* dest,char* word){
    if(!dest){
        fprintf(stderr, "Destination hash table is NULL\n");
        return;
    }
    char index = getIndexHashWord(word);

    if(index == -1){
        // pass error index
        return;
    }

    // check if word is already in hash table
    word_node* existingNode = isExistWordHash(dest, word);
    if(existingNode){
        existingNode->freq++;
        return; // Word already exists, increment frequency
    }

    word_node* newNode = createNodeWord(word,dest->str_pool, dest->node_pool);
    if(!newNode){
        fprintf(stderr, "Failed to create new node for word: %s\n", word);
        return;
    }
    if(dest->table[index] == NULL){
        // if index is empty
        dest->table[index] = newNode;
    }else{
        // add to front of list
        newNode->next = dest->table[index];
        dest->table[index] = newNode;
    }
    dest->size++;
    dest->size_each[index]++;
}

// Free the word hash table
void freeWordHash(word_hash* hash){
    if(!hash){
        error_printf("Hash table is NULL from Free Word Hash\n");
        return;
    }
    destroy_string_pool(hash->str_pool);
    freeWordNodePool(hash->node_pool);
    free(hash);
    hash = NULL;
}

void printWordHash(word_hash* hash){
    if(!hash){
        fprintf(stderr, "Hash table is NULL\n");
        return;
    }
    printf("Word Hash Table Size is %d:\n",hash->size);
    int count = 0;
    for(int i = 0; i < WORD_HASH_SIZE; i++){
        word_node* current = hash->table[i];
        while(current){
            printf("%d.Word: %s, Frequency: %d\n",count ,current->word, current->freq);
            current = current->next;
            count++;
        }
    }
}

// Pop a word from the hash table
// This function removes the word from the hash table and returns the node
word_node* pop_word(word_hash* hash, char* word){
    if(!hash || !word){
        fprintf(stderr, "Hash table or word is NULL\n");
        return NULL;
    }
    char index = getIndexHashWord(word);
    if(index == -1){
        return NULL; // Error index
    }
    word_node* current = hash->table[index];
    word_node* prev = NULL;
    while(current){
        if(memcmp(current->word, word,strlen(word)) == 0){
            // Found the word, remove it
            if(prev){
                // in the middle or end of the list
                prev->next = current->next;
            }else{
                // at the head of the list
                hash->table[index] = current->next;
            }
            current->next = NULL; // Disconnect the node
            hash->size--;
            hash->size_each[index]--;
            return current; // Word found and removed
        }
        prev = current;
        current = current->next;
    }
    return NULL; // Word not found
}

int getWordFreq(word_hash* hash, char* word){
    if(!hash || !word){
        fprintf(stderr, "Hash table or word is NULL\n");
        return -1;
    }
    word_node* node = isExistWordHash(hash, word);
    if(node){
        return node->freq; // Return frequency
    }
    return 0; // Word not found
}

void tokenize(char* str, char* token,char delim,char** save_ptr){
    char* curr = str;
    int i = 0;
    while(*curr != '\0'){
        if(*curr == delim){
            token[i] = '\0';
            *save_ptr = curr + 1;
            return;
        }
        token[i++] = *curr++;
    }
    token[i] = '\0';
    *save_ptr = NULL; // No more tokens
    return;
}

// Create a word hash from a data frame
// This function processes the text in the data frame and adds words to the hash table
word_hash* WordHash(data_frame* df){
    if(!df){
        fprintf(stderr, "Data frame is NULL\n");
        return NULL;
    }
    word_hash* hash = createWordHash();
    if(!hash){
        fprintf(stderr, "Failed to create word hash\n");
        return NULL;
    }
    char buffer[1024];buffer[0] = '\0';
    for(int i = 0; i < df->size; i++){
        char* text = df->data[i].text;
        // Process the text and add words to the hash
        char* save_ptr = NULL;
        tokenize(text, buffer, ' ', &save_ptr);
        while(buffer[0] != '\0' && save_ptr != NULL){
            if(!isStillWordEnlishIfConver(buffer)){
                buffer[0] = '\0'; // Clear the buffer
                if(save_ptr != NULL) tokenize(save_ptr, buffer, ' ', &save_ptr);
                continue; // Skip if not a valid word
            }
            push_word(hash, buffer);
            buffer[0] = '\0'; // Clear the buffer
            if(save_ptr != NULL) tokenize(save_ptr, buffer, ' ', &save_ptr);
        }
    }
    return hash;
}

// Smooth the word frequency
// remove word with frequency less than or equal smallest
// and return remove word
word_hash* smooth_word(word_hash* hash,int smallest) {
    word_hash* rt = createWordHash();
    if (!hash || !rt) {
        fprintf(stderr, "Hash table is NULL\n");
        return NULL;
    }
    for (int i = 0; i < WORD_HASH_SIZE; i++) {
        word_node* current = hash->table[i];
        word_node* prev = NULL;
        while (current) {
            if (current->freq <= smallest) {
                // Remove the node
                if (prev) {
                    prev->next = current->next;
                } else {
                    hash->table[i] = current->next;
                }
                word_node* temp = current;
                current = current->next;
                temp->next = NULL; // Disconnect the node
                // Add to the new hash table
                push_word(rt, temp->word);
                hash->size--;
            } else {
                prev = current;
                current = current->next;
            }
        }
    }
    return rt;
}

// Write the word hash to file
int writeWordHashToFile(word_hash* hash, const char* filename) {
    if (!hash || !filename) {
        fprintf(stderr, "Hash table or filename is NULL\n");
        return -1;
    }
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return -1;
    }
    for (int i = 0; i < WORD_HASH_SIZE; i++) {
        word_node* current = hash->table[i];
        fprintf(file, "-- Hash index %c:\n", i + 'a');
        while (current) {
            fprintf(file, "%s\n", current->word);
            current = current->next;
        }
    }
    fclose(file);
    return 0;
}

// Write the word hash to file
int writeWordHashToFile_Debug(word_hash* hash, const char* filename) {
    if (!hash || !filename) {
        fprintf(stderr, "Hash table or filename is NULL\n");
        return -1;
    }
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return -1;
    }
    for (int i = 0; i < WORD_HASH_SIZE; i++) {
        word_node* current = hash->table[i];
        fprintf(file, "Hash index %c:\n", i + 'a');
        while (current) {
            fprintf(file, "%s:%d\n", current->word, current->freq);
            current = current->next;
        }
    }
    fclose(file);
    return 0;
}

// Get the word form hash table
// This function returns an array of words from the hash table
// The size of the array is returned through rt_size
char** getWordFormHash(word_hash* src,int* rt_size){
    if(!src){
        fprintf(stderr, "Source hash table is NULL\n");
        exit(EXIT_FAILURE);
    }

    char** result = (char**)malloc(sizeof(char*) * (src->size+1));
    if(!result){
        fprintf(stderr, "   Memory allocation failed for result\n");
        exit(EXIT_FAILURE);
    }
    int index = 0;
    for(int i = 0; i < WORD_HASH_SIZE; i++){
        word_node* current = src->table[i];
        while(current){
            result[index] = strdup(current->word);
            index++;
            current = current->next;
        }
    }
    result[index] = NULL; // Null-terminate the array
    *rt_size = index; // Set the size of the result array
    return result;
}

// Read words from a file and add them to the hash table
// This function reads words from the specified file and adds them to the hash table
// exmpale file format:
// hi
// hello
// world
int road_word_hash(word_hash* src, const char* filename){
    if(!src || !filename){
        fprintf(stderr, "Source hash table or filename is NULL\n");
        return -1;
    }
    FILE* file = fopen(filename, "r");
    if(!file){
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return -1;
    }
    char buffer[1024];
    while(fgets(buffer, sizeof(buffer), file)){
        buffer[strcspn(buffer, "\n\r")] = '\0'; // Remove newline characters
        push_word(src, buffer);
    }
    fclose(file);
    return 0;
}

// push ngram to hash table
// This function creates n-grams from the tokens and adds them to the hash table
void push_ngram(word_hash* dest, char** tokens, int token_count, int n) {
    if (!dest || !tokens || token_count < n) return;

    char ngram[1024];
    for (int i = 0; i <= token_count - n; i++) {
        ngram[0] = '\0';
        for (int j = 0; j < n; j++) {
            strcat(ngram, tokens[i + j]);
            if (j < n - 1) strcat(ngram, "_"); // use underscore as delimiter
        }
        push_word(dest, ngram);
    }
}

word_hash* merge_wordhash(word_hash* dest, word_hash* src) {
    if (!dest || !src) {
        fprintf(stderr, "Destination or source hash table is NULL\n");
        return NULL;
    }
    for (int i = 0; i < WORD_HASH_SIZE; i++) {
        word_node* current = src->table[i];
        while (current) {
            push_word(dest, current->word);
            current = current->next;
        }
    }
    return dest;
}

void* unit_wordhash_with_ngarm(void* arg){
    thread_pack_word_hash* data = (thread_pack_word_hash*)arg;
    word_hash* hash = createWordHash();
    checkExistMemory(hash);
    StringPool* str_p = create_string_pool();
    checkExistMemory(str_p);
    char* tokens[1000]; // max 1000 tokens
    char token_buffer[128];
    char *save_ptr = NULL;
    int token_count = 0;

    for(int i = data->start; i < data->end; i++){
        char* text = data->global_df->data[i].text;
        if (!text) continue;

        save_ptr = text;
        token_count = 0;

        while (save_ptr != NULL && *save_ptr != '\0') {
            tokenize(save_ptr, token_buffer, ' ', &save_ptr);
            if (token_buffer[0] == '\0') break; // No more tokens

            if (isStillWordEnlishIfConver(token_buffer)) {
                tokens[token_count++] = str_pool_alloc(str_p, token_buffer);
            }
        }

        // push unigram
        for (int k = 0; k < token_count; k++) {
            push_word(hash, tokens[k]);
        }

        // push n-gram (e.g., bigram or trigram)
        if (data->ngram > 1) {
            push_ngram(hash, tokens, token_count, data->ngram);
        }
        reset_string_pool(str_p);
    }
    destroy_string_pool(str_p);
    data->hash = hash;
}

word_hash* WordHashWithNgram_MultiThread(data_frame* df,int n){
    if (!df) return NULL;
    word_hash* hash = createWordHash();
    checkExistMemory(hash);
    int size = df->size;
    int thread_count = 4; // Number of threads to use
    pthread_t threads[thread_count];
    thread_pack_word_hash data[thread_count];
    int chunk_size = size / thread_count;

    for (int i = 0; i < thread_count; i++) {
        data[i].start = i * chunk_size;
        data[i].end = (i == thread_count - 1) ? size : (i + 1) * chunk_size;
        data[i].hash = NULL;
        data[i].global_df = df;
        data[i].ngram = n;
        pthread_create(&threads[i], NULL, unit_wordhash_with_ngarm, &data[i]);
    }
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
        if (data[i].hash) {
            merge_wordhash(hash, data[i].hash);
            freeWordHash(data[i].hash);
            data[i].hash = NULL;
        }
    }
    // All hashes are merged in the first loop; no need for a second loop.
    return hash;
}

// Make N gram from data frame
// n is the number of words in the n-gram
// if n < 1, it will be unigram
// if n > 1 it will be n-gram
word_hash* WordHashWithNgram(data_frame* df, int n) {
    if (!df) return NULL;
    word_hash* hash = createWordHash();
    checkExistMemory(hash);

    StringPool* str_p = create_string_pool();
    checkExistMemory(str_p);
    char* tokens[1000]; // max 1000 tokens
    char token_buffer[128];
    char *save_ptr = NULL;
    int token_count = 0;
    
    for (int i = 0; i < df->size; i++) {
        char* text = df->data[i].text;
        if (!text) continue;

        save_ptr = text;
        token_count = 0;

        while (save_ptr != NULL && *save_ptr != '\0') {
            tokenize(save_ptr, token_buffer, ' ', &save_ptr);
            if (token_buffer[0] == '\0') break; // No more tokens

            if (isStillWordEnlishIfConver(token_buffer)) {
                tokens[token_count++] = str_pool_alloc(str_p, token_buffer);
            }
        }

        // push unigram
        for (int k = 0; k < token_count; k++) {
            push_word(hash, tokens[k]);
        }

        // push n-gram (e.g., bigram or trigram)
        if (n > 1) {
            push_ngram(hash, tokens, token_count, n);
        }
        reset_string_pool(str_p);
    }
    destroy_string_pool(str_p);
    return hash;
}

// like WordHashWithNgram but only for string
void String_Ngram(word_hash* origin,char* str_org, int n, StringPool* temp){
    char* text = str_org;
    checkExistMemory(text);
    char* save_ptr = text;
    char* tokens[1000]; // max 1000 tokens
    int token_count = 0;
    char token[128];token[0] = '\0';
    while (save_ptr != NULL) {
        tokenize(save_ptr, token, ' ', &save_ptr);
        if (isStillWordEnlishIfConver(token)) {
            tokens[token_count++] = str_pool_alloc(temp, token);
        }
        if (token[0] == '\0') break; // No more tokens
        if (token_count >= 1000) break; // Prevent overflow
        token[0] = '\0'; // Clear the token buffer
    }
    // push unigram
    for (int k = 0; k < token_count; k++) {
        push_word(origin, tokens[k]);
    }
    // push n-gram (e.g., bigram or trigram)
    if (n > 1) {
        push_ngram(origin, tokens, token_count, n);
    }

}

// Get the index of a word in the hash table
// This function returns the index of the word in the hash table
// The index is calculated based on the hash table size and the word's position
// If the word is not found, it returns -1
inline __attribute__((always_inline)) int getIndexOfWord(word_hash* hash, char* word){
    if(!hash || !word){
        fprintf(stderr, "Hash table or word is NULL\n");
        return -1;
    }
    char index = getIndexHashWord(word);
    if(index == -1){
        return -1; // Error index
    }
    word_node* current = hash->table[index];
    int i = 0;
    int offset = 0;
    for(int i = 0;i < index;i++){
        offset += hash->size_each[i];
    }
    while(current){
        if(memcmp(current->word, word,strlen(word)) == 0 && strlen(current->word) == strlen(word)){
            return i + offset; // Word found at index i
        }
        current = current->next;
        i++;
    }
    return -1; // Word not found
}

inline __attribute__((always_inline)) char* getWordFromIndex(word_hash* hash, int index){
    if(!hash || index < 0 || index >= hash->size){
        fprintf(stderr, "Hash table is NULL or index out of bounds\n");
        return NULL;
    }
    int offset = 0;
    for(int i = 0; i < WORD_HASH_SIZE; i++){
        if(offset + hash->size_each[i] > index){
            word_node* current = hash->table[i];
            int j = 0;
            while(current && j < (index - offset)){
                current = current->next;
                j++;
            }
            if(current){
                return current->word; // Return the word at the specified index
            }else{
                return NULL; // Index out of bounds
            }
        }
        offset += hash->size_each[i];
    }
    return NULL; // Index not found
}