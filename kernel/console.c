#include <kernel/console.h>
#include <lib/print.h>


static struct lock console_lock; //终端锁

/**
 * 初始化终端
*/
void console_init(void) 
{ 
  lock_init (&console_lock); 
}

/**
 * 用户获取终端
*/
void console_acquire(void)
{
  lock_acquire (&console_lock); 
}

/**
 * 用户释放终端
*/
void console_release(void)
{ 
  lock_release (&console_lock); 
}

/**
 * 终端中输出字符串
*/
void console_put_str(char* str) 
{ 
  console_acquire() ; 
  s_putstr(str); 
  console_release() ; 
} 
/**
 * 终端输出字符
*/
void console_put_char(uint8_t char_asci) 
{ 
  console_acquire(); 
  s_putchar(char_asci); 
  console_release(); 
} 

/**
 * 终端中输出十六进制整数
*/
void console_put_int (uint32_t num) 
{ 
  console_acquire(); 
  s_putnum(num); 
  console_release(); 
} 

