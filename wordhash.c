#include "wordhash.h"

// This function is used to create a new word_node
// It will allocate memory for the new node and initialize it
word_node* createNodeWord(char* word){
    word_node* newNode = (word_node*)malloc(sizeof(word_node));
    if(!newNode){
        fprintf(stderr, "Memory allocation failed for newNode\n");
        return NULL;
    }
    newNode->word = strdup(word);
    newNode->freq = 1;
    newNode->next = NULL;
    return newNode;
}

// De allocate memory for word_node
void freeNodeWord(word_node* node){
    if(node){
        free(node->word);
        free(node);
    }
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
    return newHash;
}

// Convert first character of word to English
// Return the index for the hash table
char getIndexHashWord(char* word){
    if(!word){
        fprintf(stderr, "Word is NULL\n");
        return -1;
    }
    char index = 0;
    // key is first char
    char temp = covert_to_eng(word);
    if(temp == '\0'){
        fprintf(stderr, "Invalid character in word: %s is \\0 in line: %d in file: %s\n", word, __LINE__, __FILE__);
        exit(EXIT_FAILURE);
    }

    // if is digit pass
    if(isdigit(temp)){
        fprintf(stderr, "Invalid character in word: %s is digit in line: %d in file: %s\n", word, __LINE__, __FILE__);
        return -1; // Invalid index for digits
    }

    index = tolower(temp) - 'a';
    if(index < 0 || index >= WORD_HASH_SIZE){
        fprintf(stderr, "Index out of bounds for word: %s\n", word);
        return -1; // Invalid index
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
        exit(EXIT_FAILURE); // Invalid index
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
        // pass digit
        return; // Invalid index for digits
    }

    // check if word is already in hash table
    word_node* existingNode = isExistWordHash(dest, word);
    if(existingNode){
        existingNode->freq++;
        return; // Word already exists, increment frequency
    }

    word_node* newNode = createNodeWord(word);
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

// Copy the word hash table
// Create a new hash table and copy the contents of the original
word_hash* copyWordHash(word_hash* original){
    if(!original){
        fprintf(stderr, "Original hash table is NULL\n");
        return NULL;
    }
    word_hash* newHash = createWordHash();
    if(!newHash){
        fprintf(stderr, "Memory allocation failed for newHash\n");
        return NULL;
    }
    for(int i = 0; i < WORD_HASH_SIZE; i++){
        word_node* current = original->table[i];
        while(current){
            push_word(newHash, current->word);
            current = current->next;
        }
    }
    return newHash;
}

// Free the word hash table
void freeWordHash(word_hash* hash){
    if(!hash){
        fprintf(stderr, "Hash table is NULL\n");
        return;
    }
    for(int i = 0; i < WORD_HASH_SIZE; i++){
        word_node* current = hash->table[i];
        while(current){
            word_node* temp = current;
            current = current->next;
            freeNodeWord(temp);
        }
    }
    free(hash);
}

void printWordHash(word_hash* hash){
    if(!hash){
        fprintf(stderr, "Hash table is NULL\n");
        return;
    }
    printf("Word Hash Table:\n");
    int count = 0;
    for(int i = 0; i < WORD_HASH_SIZE; i++){
        word_node* current = hash->table[i];
        while(current){
            printf("%d.Word: %s, Frequency: %d\n",count ,current->word, current->freq);
            current = current->next;
            count++;
        }
    }
    printf("Word Hash Table Size: %d\n", hash->size);
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
        return NULL; // Invalid index
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
    for(int i = 0; i < df->size; i++){
        char* text = strdup(df->data[i].text);
        if(!text){
            fprintf(stderr, "Memory allocation failed for text\n");
            freeWordHash(hash);
            return NULL;
        }
        char* save = text;
        // Process the text and add words to the hash
        char* save_ptr = NULL;
        char* token = strtok_r(text, " ", &save_ptr);
        while(token){
            if(!isStillWordEnlishIfConver(token)){
                token = strtok_r(NULL, " ", &save_ptr);
                continue; // Skip if not a valid word
            }
            push_word(hash, token);
            token = strtok_r(NULL, " ", &save_ptr);
        }
        free(save); // Free the duplicated string
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
                freeNodeWord(temp); // Free the removed node
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
        fprintf(file, "Hash index %c:\n", i + 'a');
        while (current) {
            fprintf(file, "     { %s , %d }\n", current->word, current->freq);
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
    }else{
        printf("    Memory allocation for result successful with size: %d\n", src->size+1);
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

// Make N gram from data frame
// n is the number of words in the n-gram
// if n < 1, it will be unigram
// if n > 1 it will be n-gram
word_hash* WordHashWithNgram(data_frame* df, int n) {
    if (!df) return NULL;
    word_hash* hash = createWordHash();

    for (int i = 0; i < df->size; i++) {
        char* text = strdup(df->data[i].text);
        if (!text) continue;

        char* save_ptr = NULL;
        char* token = strtok_r(text, " ", &save_ptr);
        char* tokens[1000]; // max 1000 tokens
        int token_count = 0;

        while (token) {
            if (isStillWordEnlishIfConver(token)) {
                tokens[token_count++] = token;
            }
            token = strtok_r(NULL, " ", &save_ptr);
        }

        // push unigram
        for (int k = 0; k < token_count; k++) {
            push_word(hash, tokens[k]);
        }

        // push n-gram (e.g., bigram or trigram)
        if (n > 1) {
            push_ngram(hash, tokens, token_count, n);
        }

        free(text);
    }
    return hash;
}

// like WordHashWithNgram but only for string
word_hash* String_Ngram(char* str_org, int n){
    char* text = strdup(str_org);
    checkExistMemory(text);
    word_hash* hash = createWordHash();
    checkExistMemory(hash);
    char* save_ptr = NULL;
    char* token = strtok_r(text, " ", &save_ptr);
    char* tokens[1000]; // max 1000 tokens
    int token_count = 0;

    while (token) {
        if (isStillWordEnlishIfConver(token)) {
            tokens[token_count++] = token;
        }
        token = strtok_r(NULL, " ", &save_ptr);
    }
    // push unigram
    for (int k = 0; k < token_count; k++) {
        push_word(hash, tokens[k]);
    }
    // push n-gram (e.g., bigram or trigram)
    if (n > 1) {
        push_ngram(hash, tokens, token_count, n);
    }
    free(text);
    return hash;
}

// Get the index of a word in the hash table
// This function returns the index of the word in the hash table
// The index is calculated based on the hash table size and the word's position
// If the word is not found, it returns -1
int getIndexOfWord(word_hash* hash, char* word){
    if(!hash || !word){
        fprintf(stderr, "Hash table or word is NULL\n");
        return -1;
    }
    char index = getIndexHashWord(word);
    if(index == -1){
        return -1; // Invalid index
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
