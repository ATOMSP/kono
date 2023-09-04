#ifndef __LIB_IO_H
#define __LIB_IO_H


#ifdef __cplusplus
extern "C"{
#endif

/* c语言指定读写IO口函数 */
#include "../kernel/stdint.h"


/**
 * 向指定端口写入一个字节(N:0~255 b:写入字节 w:写入端口2个字节)
*/
static inline void outb(uint16_t port,uint8_t dat){
    asm volatile("outb %b0,%w1"::"a"(dat),"Nd"(port));
}

/**
 * 向指定端口写入指定的数量的字: 使用了edi ecx dx（两个字节为单位）
*/
static inline void outsw(uint16_t port,const void* src,uint32_t len){
    asm volatile("cld; \
                  rep outsw"
                  :"+S"(src),"+c"(len)
                  :"d"(port));
}

/**
 * 向指定端口读入一个字节（N：0～255 b:读入字节）
*/
static inline uint8_t inb(uint16_t port){
    uint8_t dat;
    asm volatile("inb %w1,%b0":"=a"(dat):"Nd"(port));
    return dat;
}

/**
 * 向指定端口读入指定数量的字(两个字节为单位)
*/
static inline void insw(uint16_t port,const void* desc,uint32_t len){
    asm("cld;\
         rep insw"
         :"+D"(desc),"+c"(len)
         :"d"(port)
         :"memory");
}



#ifdef __cplusplus
}
#endif





#endif





