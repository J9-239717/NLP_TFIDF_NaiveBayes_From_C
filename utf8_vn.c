#include "utf8_vn.h"

// Mapping Vietnamese characters to English equivalents
ToneMapping viet_mapping_eng[] = {
    {"â", 'a'}, {"ă", 'a'}, {"ê", 'e'}, {"ô", 'o'}, {"ơ", 'o'}, {"ư", 'u'}, {"đ", 'd'},
    {"á", 'a'}, {"à", 'a'}, {"ả", 'a'}, {"ã", 'a'}, {"ạ", 'a'},
    {"ấ", 'a'}, {"ầ", 'a'}, {"ẩ", 'a'}, {"ẫ", 'a'}, {"ậ", 'a'},
    {"ắ", 'a'}, {"ằ", 'a'}, {"ẳ", 'a'}, {"ẵ", 'a'}, {"ặ", 'a'},
    {"é", 'e'}, {"è", 'e'}, {"ẻ", 'e'}, {"ẽ", 'e'}, {"ẹ", 'e'},
    {"ế", 'e'}, {"ề", 'e'}, {"ể", 'e'}, {"ễ", 'e'}, {"ệ", 'e'},
    {"í", 'i'}, {"ì", 'i'}, {"ỉ", 'i'}, {"ĩ", 'i'}, {"ị", 'i'},
    {"ó", 'o'}, {"ò", 'o'}, {"ỏ", 'o'}, {"õ", 'o'}, {"ọ", 'o'},
    {"ố", 'o'}, {"ồ", 'o'}, {"ổ", 'o'}, {"ỗ", 'o'}, {"ộ", 'o'},
    {"ớ", 'o'}, {"ờ", 'o'}, {"ở", 'o'}, {"ỡ", 'o'}, {"ợ", 'o'},
    {"ú", 'u'}, {"ù", 'u'}, {"ủ", 'u'}, {"ũ", 'u'}, {"ụ", 'u'},
    {"ứ", 'u'}, {"ừ", 'u'}, {"ử", 'u'}, {"ữ", 'u'}, {"ự", 'u'},
    {"ý", 'y'}, {"ỳ", 'y'}, {"ỷ", 'y'}, {"ỹ", 'y'}, {"ỵ", 'y'}
};

// Function to find the base character of a Vietnamese character
char get_base_telex(const char* c) {
    for(int i = 0; i < TONE_MAPPING_SIZE; i++){
        if(strcmp(viet_mapping_eng[i].viet_char,c) == 0){
            return viet_mapping_eng[i].base_char;
        }
    }
    return '\0';
}

// check frist char is vietnamese or english
// if is vietnamese will covert to english
// and return first char
char covert_to_eng(const char* str){
    char utf8_char[4] = {0};
    int char_len = 1;

    // check first char is english
    if ((*str & 0x80) == 0) {
        return *str; // English character
    }

    // Check if it's a multi-byte UTF-8 character
    if ((*str & 0xE0) == 0xC0) char_len = 2; // 2-byte character
    else if ((*str & 0xF0) == 0xE0) char_len = 3; // 3-byte character

    strncpy(utf8_char, str, char_len); // Copy the character

    char base_char = '\0';
    base_char = get_base_telex(utf8_char);
    if(!base_char){
        return *str;
    }
    return base_char;
}

int isStillWordEnlishIfConver(char * str){
    // check if is digit
    if(isdigit(*str)){
        return 0;
    }

    char c = covert_to_eng(str);
    if(c == '\0'){
        return 0; // Invalid character
    }
    if(isdigit(c)){
        return 0; // Invalid character
    }

    return 1;
}