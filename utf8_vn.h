#ifndef _UTF8_VN_H
#define _UTF8_VN_H

#include "import.h"

// Struct to map Vietnamese to English
typedef struct {
    char* viet_char;
    char base_char; 
} ToneMapping;

extern ToneMapping viet_mapping_eng[];

#define TONE_MAPPING_SIZE (sizeof(viet_mapping_eng) / sizeof(viet_mapping_eng[0]))

char get_base_telex(const char* c);
char covert_to_eng(const char* str);
int isStillWordEnlishIfConver(char * str);

#endif