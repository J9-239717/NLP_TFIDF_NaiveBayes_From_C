#include "h_files/utf8_vn.h"

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

ToneMapping viet_mapping_eng_upper[] = {
    {"Â", 'A'}, {"Ă", 'A'}, {"Ê", 'E'}, {"Ô", 'O'}, {"Ơ", 'O'}, {"Ư", 'U'}, {"Đ", 'D'},
    {"Á", 'A'}, {"À", 'A'}, {"Ả", 'A'}, {"Ã", 'A'}, {"Ạ", 'A'},
    {"Ấ", 'A'}, {"Ầ", 'A'}, {"Ẩ", 'A'}, {"Ẫ", 'A'}, {"Ậ", 'A'},
    {"Ắ", 'A'}, {"Ằ", 'A'}, {"Ẳ", 'A'}, {"Ẵ", 'A'}, {"Ặ", 'A'},
    {"É", 'E'}, {"È", 'E'}, {"Ẻ", 'E'}, {"Ẽ", 'E'}, {"Ẹ", 'E'},
    {"Ế", 'E'}, {"Ề", 'E'}, {"Ể", 'E'}, {"Ễ", 'E'}, {"Ệ", 'E'},
    {"Í", 'I'}, {"Ì", 'I'}, {"Ỉ", 'I'}, {"Ĩ", 'I'}, {"Ị", 'I'},
    {"Ó", 'O'}, {"Ò", 'O'}, {"Ỏ", 'O'}, {"Õ", 'O'}, {"Ọ", 'O'},
    {"Ố", 'O'}, {"Ồ", 'O'}, {"Ổ", 'O'}, {"Ỗ", 'O'}, {"Ộ", 'O'},
    {"Ớ", 'O'}, {"Ờ", 'O'}, {"Ở", 'O'}, {"Ỡ", 'O'}, {"Ợ", 'O'},
    {"Ú", 'U'}, {"Ù", 'U'}, {"Ủ", 'U'}, {"Ũ", 'U'}, {"Ụ", 'U'},
    {"Ứ", 'U'}, {"Ừ", 'U'}, {"Ử", 'U'}, {"Ữ", 'U'}, {"Ự", 'U'},
    {"Ý", 'Y'}, {"Ỳ", 'Y'}, {"Ỷ", 'Y'}, {"Ỹ", 'Y'}, {"Ỵ", 'Y'}
};


// Function to find the base character of a Vietnamese character
char get_base_telex(const char* c) {
    for(int i = 0; i < TONE_MAPPING_SIZE; i++){
        if(strcmp(viet_mapping_eng[i].viet_char,c) == 0){
            return viet_mapping_eng[i].base_char;
        }
    }
    for(int i = 0; i < TONE_MAPPING_SIZE; i++){
        if(strcmp(viet_mapping_eng_upper[i].viet_char,c) == 0){
            return viet_mapping_eng_upper[i].base_char;
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

inline __attribute__((always_inline)) int isStillWordEnlishIfConver(char * str){
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