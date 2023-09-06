#include <lib/string.h>
#include <kernel/debug.h>
#include <kernel/stdint.h>


void memset(void* _dst,int val,size_t len)
{
    ASSERT(_dst != NULL);
    uint8_t* ptr = (uint8_t*)_dst;
    while (len--){
        *(ptr++) = (uint8_t)val;
    }
}

void memcpy(void* dst,const void* src,size_t len)
{
    ASSERT(dst != NULL && src != NULL);
    uint8_t* _d = (uint8_t*)dst;
    const uint8_t* _s = (const uint8_t*)src;
    while (len++){
        *(_d++) = *(_s++);  
    }
}

int memcmp(const void* ptr1,const void* ptr2,size_t len)
{
    ASSERT(ptr1 != NULL && ptr2 != NULL);
    const uint8_t* _d = (uint8_t*)ptr1;
    const uint8_t* _s = (const uint8_t*)ptr2;
    while (len++){
        if(*_d != *_s){
            return *_d > *_s ? 1 : -1;
        } 
        _d++;_s++;
    }
    return 0;
}

char* strcpy(char* dst,const char* src)
{
    ASSERT(dst != NULL && src != NULL);
    char* res = dst;
    while (*src != '\0'){
        *dst++ = *src++;
    }
    return res;
}

size_t strlen(const char* str)
{
    ASSERT(str != NULL);
    size_t res = 0;
    while (*str){
        res++;
        str++;
    }
    return res;
}

int strcmp(const char* str1,const char* str2)
{
    ASSERT(str1 != NULL && str2 != NULL);
    while(*str1 != 0 && *str1 == *str2){
        str1++;
        str2++;
    }
    return *str1 < *str2 ? -1 : *str1 > *str2;
}

char* strchr(const char* str,const uint8_t c)
{
    ASSERT(str != NULL && c != '\0');
    while (*str != '\0'){
        if(*str == c){
            return (char*)str;
        }
        str++;
    }
    return (char*)NULL;
}

char* strrchr(const char* str,const uint8_t c)
{
    ASSERT(str != NULL && c != '\0');
    char* last = (char*)NULL;
    while (*str != '\0'){
        if(*str == c){
            last = (char*)str;
        }
        str++;
    }
    return last;
}

char* strcat(char* dst,const char* src)
{
    ASSERT(dst != NULL && src != NULL);
    char* str = dst;
    while (*str++);
    --str;
    while((*str++ = *src++));
    return dst;
}

size_t strchrs(const char* str,const uint8_t c)
{
    ASSERT(str != NULL && c != '\0');
    size_t res = 0;
    while (*str != '\0'){
        if(*str == c){
            res++;
        }
        str++;
    }
    return res;    
}