#ifndef __KERNEL_CONSOLE_H
#define __KERNEL_CONSOLE_H

#ifdef __cplusplus
extern "C"{
#endif

#include <kernel/stdint.h>
#include <lib/thread.h>
#include <lib/sync.h>

/**
 * 初始化终端
*/
void console_init(void);
/**
 * 终端中输出字符串
*/
void console_put_str(char* str); 
/**
 * 终端输出字符
*/
void console_put_char(uint8_t char_asci) ;
/**
 * 终端中输出十六进制整数
*/
void console_put_int (uint32_t num); 


#ifdef __cplusplus
}
#endif


#endif

