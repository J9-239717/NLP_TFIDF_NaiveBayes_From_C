#include "wordhash.h"

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

void freeNodeWord(word_node* node){
    if(node){
        free(node->word);
        free(node);
    }
}

word_hash* createWordHash(){
    word_hash* newHash = (word_hash*)malloc(sizeof(word_hash));
    if(!newHash){
        fprintf(stderr, "Memory allocation failed for newHash\n");
        return NULL;
    }
    newHash->size = 0;
    for(int i = 0; i < WORD_HASH_SIZE; i++){
        newHash->table[i] = NULL;
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
        fprintf(stderr, "Invalid character in word: %s\n", word);
        exit(EXIT_FAILURE);
    }

    // if is digit pass
    if(isdigit(temp)){
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
        exit(EXIT_FAILURE); // Invalid index
    }

    word_node* current = hash->table[index];
    while(current){
        if(memcmp(current->word, word,strlen(word)) == 0){
            return current; // Word exists
        }
        current = current->next;
    }
    return NULL; // Word does not exist
}

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
}

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
    for(int i = 0; i < WORD_HASH_SIZE; i++){
        word_node* current = hash->table[i];
        while(current){
            printf("Word: %s, Frequency: %d\n", current->word, current->freq);
            current = current->next;
        }
    }
    printf("Word Hash Table Size: %d\n", hash->size);
}

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
            if(prev){
                // in the middle or end of the list
                prev->next = current->next;
            }else{
                // at the head of the list
                hash->table[index] = current->next;
            }
            current->next = NULL; // Disconnect the node
            hash->size--;
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

char** getWordFormHash(word_hash* src,int* rt_size){
    if(!src){
        fprintf(stderr, "Source hash table is NULL\n");
        return NULL;
    }

    char** result = (char**)malloc(sizeof(char*) * (src->size+1));
    if(!result){
        fprintf(stderr, "Memory allocation failed for result\n");
        return NULL;
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