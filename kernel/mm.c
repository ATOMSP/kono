#include <kernel/mm.h>
#include <kernel/stdint.h>
#include <lib/bitmap.h>
#include <lib/print.h>
#include <lib/string.h>
#include <kernel/debug.h>

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

/**
 * 向特定虚拟内存池申请pn个虚拟内存页，成功返回虚拟内存首地址，失败则返回null
*/
static void* vaddr_get(enum Pool_Flag pool,uint32_t pn)
{
  int offset = -1,_addr = 0;
  uint32_t cnt = 0;
  //判断所用内存池
  if(pool == PF_KERNEL){
    offset = Bitmap_scan(&(kernel_vmm._map),pn);
    if(offset == -1){
      s_putstr("[Error]:out of kernel memory!\n");
      return NULL;
    }else{
      _addr = (kernel_vmm._addr + (offset) * PAGE_SIZE);
      while(cnt < pn){
        Bitmap_set_bit(&(kernel_vmm._map),offset + cnt++,1);
      }
    }
  }else if(pool == PF_USER){
    //之后用
  }else{
    s_putstr("Unknown mem pool!\n");
    while(1);
  }
  return (void*)_addr;
}


/**
 * 得到虚拟地址对应的页表项虚拟地址指针(pte相当于页头地址，该虚拟地址就存在在这个页中)
*/
static size_t* getVaddrPtePtr(size_t vaddr)
{
  uint32_t * pte = (uint32_t*)(0xffc00000 + ((vaddr & 0xffc00000) >> 10) + (PTE_IDX(vaddr) * 4));
  return pte;
}
/**
 * 得到虚拟地址所对应的页目录项虚拟地址的指针
*/
static size_t* getVaddrPdePtr(size_t vaddr)
{
  uint32_t * pde = (uint32_t*)(0xfffff000 + (PDE_IDX(vaddr) * 4));
  return pde;
}

/**
 * 在指定物理内存池中申请一页物理内存：成功返回物理首地址 失败返回null
*/
static void* palloc(struct pool* m_pool)
{
  ASSERT(m_pool != NULL);
  //这里传进来的m_pool存在问题，貌似没影响后面
  int offset = Bitmap_scan(&(m_pool->pool_map),1);
  if(offset == -1){
    s_putstr("[Error]: out of memory!\n");
    return NULL;
  }else{
    Bitmap_set_bit(&(m_pool->pool_map),offset,1);
  }

  return (void*)(m_pool->addr + (offset * PAGE_SIZE));
}


/**
 * 建立vaddr虚拟地址到paddr物理地址的映射
*/
static void page_table_mapping(void* vaddr,void* paddr)
{
  uint32_t _vaddr = (uint32_t)vaddr,_paddr = (uint32_t)paddr;
  //获取虚拟地址的pde和pte的虚拟地址，以便于后续访问
  uint32_t * pte = getVaddrPtePtr(_vaddr);
  uint32_t * pde = getVaddrPdePtr(_vaddr);
  //判断该虚拟地址所对应的页目录项是否存在，也就是有没有建表
  if(*pde & 0x00000001){
    //该页表存在，开始判断页表项存不存在
    ASSERT(!(*pte & 0x00000001));
    if(!(*pte & 0x00000001)){
      *pte = (_paddr | PG_US_U | PG_RW_W | PG_P_1);//填上物理地址
    }else{
      PANIC("pte repeat!");
      *pte = (_paddr | PG_US_U | PG_RW_W | PG_P_1);//填上物理地址
    }
  }else{
    //页目录项不存在，表示目录都没建
    //新建页目录，并且申请一页内存存放新页表，并且对该内存清零
    uint32_t pde_paddr = (uint32_t)palloc(&kernel_pool);
    //为pde填上新的页表物理地址
    *pde = (pde_paddr | PG_US_U | PG_RW_W | PG_P_1);
    //这里清除了页表!!，因为此时pte是虚拟地址对应的页表项的虚拟地址，这里&上0xfffff000是为了取到页表项对应的页表地址
    //这里很绕
    memset((void*)((int)pte & 0xfffff000),0,PAGE_SIZE);
    //断言该虚拟地址对应的页表项没有映射过
    ASSERT(!(*pte & 0x00000001));
    *pte = (_paddr | PG_US_U | PG_RW_W | PG_P_1);
  }
}

/**
 * 分配pn个页空间，成功则返回虚拟首地址
 * 1. 申请虚拟地址
 * 2. 申请物理地址
 * 3. 地址映射
*/
void * malloc_page(enum Pool_Flag pf,uint32_t pn)
{
  //15MB，内核内存池大小
  ASSERT(pn > 0 && pn < 3840);
  void* vaddr_start = vaddr_get(pf,pn);
  if(vaddr_start == NULL){
    s_putstr("vaddr get failed!\n");
    return NULL;
  }
  uint32_t vaddr = (uint32_t)vaddr_start,cnt = pn;
  //获取内存池指针
  struct pool * mem_pool = pf & PF_KERNEL ? &kernel_pool : &user_pool;
  while(cnt-- > 0){
    //申请物理内存
    void * page_paddr = palloc(mem_pool);
    if(page_paddr == NULL){
      //这里还需要处理...
      s_putstr("[Error]:phy memory get failed!\n");
      return NULL;
    }
    //映射地址
    page_table_mapping((void*)vaddr,page_paddr);
    vaddr += PAGE_SIZE;
  }
  return vaddr_start;
}


/**
 * 从内核空间申请pn个页内存：成功则返回虚拟地址头，失败返回null
*/
void * get_kernel_pages(uint32_t pn)
{
  s_putstr("get kernel pages ...\n");
  void * vaddr_start = malloc_page(PF_KERNEL,pn);
  if(vaddr_start == NULL){
    return NULL;
  }
  s_putstr("get vaddr is 0x");
  s_putnum((uint32_t)vaddr_start);
  s_putchar('\n');
  //将申请到的内存清零备用
  memset(vaddr_start,0,PAGE_SIZE * pn);
  return vaddr_start;
}



