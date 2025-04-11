#ifndef _STRING_POOL_H_
#define _STRING_POOL_H_

#include "import.h"
#include "structure_data.h"

StringPool* create_string_pool();
void destroy_string_pool(StringPool* pool);
char* str_pool_alloc(StringPool* pool,const char* str);
void reset_string_pool(StringPool* pool);

#endif