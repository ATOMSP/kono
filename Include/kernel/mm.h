#ifndef __KERNEL_MM_H 
#define __KERNEL_MM_H 

#ifdef __cplusplus
extern "C"{
#endif

#include <lib/bitmap.h>
#include <kernel/stdint.h>

//内存一页字节数
#define PAGE_SIZE 4096
//内核pool首地址
#define KERNEL_HEAP_ADDR 0xc0100000


struct virtual_memory_manager{
  struct bitmap _map;
  size_t _addr;
};


extern struct pool kernel_pool,user_pool;
void init_mem(void);




#ifdef __cplusplus
}
#endif


#endif



