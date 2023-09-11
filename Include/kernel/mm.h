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
//位图地址
#define MEM_BITMAP_BASE_ADDR 0xc009a000 //最大支持4个页框大小的位图，支持512MB

//物理内存池标志
enum Pool_Flag{
  PF_KERNEL = 1,
  PF_USER = 2
};
//页目录/页表项属性
#define PG_P_1    1 //存在属性
#define PG_P_0    0
#define PG_RW_R   0// 读写属性
#define PG_RW_W   2
#define PG_US_U   4//系统非系统属性
#define PG_US_S   0


/**
 * 虚拟地址管理
*/
struct virtual_memory_manager
{
  struct bitmap _map;
  size_t _addr;
};


/**
 * 用于管理物理内存
*/
struct pool
{
  struct bitmap pool_map;
  size_t addr;
  size_t capacity;
};

extern struct pool kernel_pool,user_pool;
/**
 * 初始化内存
*/
void init_mem(void);



#ifdef __cplusplus
}
#endif


#endif



