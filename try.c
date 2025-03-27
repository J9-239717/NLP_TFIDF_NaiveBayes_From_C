#include "import.h"
#include "utf8_vn.h"


int main(){
    char* str[] = {
        "án",
        "b",
        "êng",
        "động",
        "z",
        "â",
        "ận",
        "ỹ"
    };

    for(int i = 0; i < sizeof(str)/sizeof(str[0]); i++){
        printf("%c\n",covert_to_eng(str[i]));
    }
}