#include <kernel/stdint.h>
#include <kernel/macro.h>
#include <lib/thread.h>



#define PG_SIZE  4096

/*  TSS结构 */
struct tss{ 
  uint32_t backlink; //上一个TSS指针
  uint32_t* esp0; 
  uint32_t  ss0; 
  uint32_t* esp1; 
  uint32_t  ss1; 
  uint32_t* esp2; 
  uint32_t  ss2; 
  uint32_t  cr3; 
  uint32_t  (*eip)(void) ; 
  uint32_t  eflags; 
  uint32_t  eax; 
  uint32_t  ecx; 
  uint32_t  edx; 
  uint32_t  ebx; 
  uint32_t  esp; 
  uint32_t  ebp; 
  uint32_t  esi; 
  uint32_t  edi; 
  uint32_t  es; 
  uint32_t  cs; 
  uint32_t  ss; 
  uint32_t  ds; 
  uint32_t  fs; 
  uint32_t  gs; 
  uint32_t  ldt;  
  uint32_t  trace; 
  uint32_t  io_base; 
} ; 
/* tss结构 */
static struct tss tss;

/**
 * 更新tss的esp0字段的值为线程的0级栈
*/
void update_tss_esp(struct task_struct * pthread)
{
  tss.esp0 = (uint32_t*)((uint32_t)pthread + PG_SIZE);
}

/**
 * 创建GDT描述符
*/
static struct gdt_desc make_gdt_desc(
  uint32_t * desc_addr,
  uint32_t   limit,
  uint8_t    attr_low,
  uint8_t    attr_high){
  //得到描述符的基地址
  uint32_t desc_base = (uint32_t)desc_addr;
  struct gdt_desc desc;
  desc.limit_low_word = limit & 0x0000ffff;
  desc.base_low_word = desc_base & 0x0000ffff;
  desc.base_mid_byte = ((desc_base & 0x00ff0000));

}





