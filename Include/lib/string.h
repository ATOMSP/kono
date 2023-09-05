#ifndef __LIB_STRING_H
#define __LIB_STRING_H


#ifdef __cplusplus
extern "C"{
#endif

#include "../kernel/stdint.h"

#define NULL ((void*)0)



void memset(void* _dst,int val,size_t len);
void memcpy(void* dst,const void* src,size_t len);
int memcmp(const void* ptr1,const void* ptr2,size_t len);
char* strcpy(char* dst,const char* src);
size_t strlen(const char* str);
int strcmp(const char* str1,const char* str2);
char* strchr(const char* str,const uint8_t c);
char* strrchr(const char* str,const uint8_t c);
char* strcat(char* dst,const char* src);
size_t strchrs(const char* str,const uint8_t c);



#ifdef __cplusplus
}
#endif




#endif










