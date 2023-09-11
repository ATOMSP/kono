#include <kernel/mm.h>
#include <kernel/stdint.h>
#include <lib/bitmap.h>
#include <lib/print.h>


//内核物理内存池，用户物理内存池
struct pool kernel_pool,user_pool;
//内核虚拟内存池
struct virtual_memory_manager kernel_vmm; 


 /**
  * 内存管理初始化:此时物理内存分好页表可使用的内存区域是0x102000往上
 */
void init_mem_pool(uint32_t mem_all)
{
  s_putstr("mem pool init ...\n");
  //目前共创建了1页页目录表项指向一个页表 + 第0页和第768页存放同一个页表 + 769
  //~1022个页目录项指向254个页表，所以共256个页框（注意用了257个页目录项，指向
  //了256个页表）
  //物理内存
  uint32_t page_table_size = PAGE_SIZE * 256;//计算已创建的页表总空间0x100000（页表所占空间）
  uint32_t used_mem = page_table_size + 0x100000;//计算目前已掌握的空间
  uint32_t free_mem = mem_all - used_mem;//计算剩余未处理的空间
  uint16_t free_pages = free_mem / PAGE_SIZE;//计算未处理空间的所代表的页数
  uint16_t kernel_free_pages = free_pages / 2;//分配内核物理空间
  uint16_t user_free_pages = free_pages - kernel_free_pages;//分配用户物理空间
  uint32_t kbm_length = kernel_free_pages / 8;//得到内核位图长度
  uint32_t ubm_length = user_free_pages / 8;//得到用户位图长度
  //此时内核与用户的可管理的内存空间尚未建立页表映射
  uint32_t kp_base_addr = used_mem;//内核物理空间起始地址从内存0x200000开始
  //用户物理空间起始地址紧随内核空间之后
  uint32_t up_base_addr = kp_base_addr + kernel_free_pages * PAGE_SIZE;
  //初始化内核与用户的物理内存池
  kernel_pool.addr = kp_base_addr;
  kernel_pool.capacity = kernel_free_pages * PAGE_SIZE;
  kernel_pool.pool_map.map_lenb = kbm_length;

  user_pool.addr = up_base_addr;
  user_pool.capacity = user_free_pages * PAGE_SIZE;
  user_pool.pool_map.map_lenb = ubm_length;

  //将内核位图和用户位图放置低1M地址的特定位置  
  kernel_pool.pool_map.bits = (void*)MEM_BITMAP_BASE_ADDR;
  user_pool.pool_map.bits = (void*)(MEM_BITMAP_BASE_ADDR + kbm_length);

  //显示内存信息  
  s_putstr("-----------------\n");
  s_putstr("kernel bitmap base addr:0x");
  s_putnum((int)kernel_pool.pool_map.bits);
  s_putstr("\nkernel phy base addr:0x");
  s_putnum(kernel_pool.addr);
  s_putstr("\nuser bitmap base addr:0x");
  s_putnum((int)user_pool.pool_map.bits);
  s_putstr("\nuser phy base addr:0x");
  s_putnum(user_pool.addr);
  s_putstr("\n-----------------\n\t");

  //初始化内核与用户物理内存池位图
  Bitmap_init(&(kernel_pool.pool_map));
  Bitmap_init(&(user_pool.pool_map));
  //初始化内核虚拟内存管理
  kernel_vmm._map.map_lenb = kbm_length;//长度与物理位图长度一致
  //位图指向内核用户物理位图之后
  kernel_vmm._map.bits = (void*)(MEM_BITMAP_BASE_ADDR + kbm_length + ubm_length);
  //定位虚拟地址起始位置
  kernel_vmm._addr = KERNEL_HEAP_ADDR;
  Bitmap_init(&(kernel_vmm._map));
  s_putstr("init mem pool ok!\n");
}

/**
 * 初始化内存
*/
void init_mem(void)
{
  s_putstr("init mem ...\n");
  uint32_t mem_capacity = *(uint32_t*)(0xb09);//获取内存容量
  init_mem_pool(mem_capacity);
  s_putstr("init mem ok!\n");
}




