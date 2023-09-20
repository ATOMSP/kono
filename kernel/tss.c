#include <kernel/stdint.h>
#include <kernel/macro.h>
#include <lib/thread.h>
#include <lib/print.h>
#include <lib/string.h>
#include <kernel/tss.h>


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
  desc.base_mid_byte = ((desc_base & 0x00ff0000) >> 16);
  desc.attr_low_byte = (uint8_t)attr_low;
  desc.limit_high_attr_high = (((limit & 0x000f0000) >> 16) + 
  (uint8_t)(attr_high));
  desc.base_high_byte = desc_base >> 24;
  return desc;
}

/**
 * 在GDT中创建tss并重新加载GDT
*/
void tss_init(void)
{
  s_putstr("tss init...\n");
  uint32_t tss_size = sizeof(tss);
  memset(&tss,0,tss_size);
  tss.ss0 = SELECTOR_K_STACK;
  tss.io_base = tss_size;
  *((struct gdt_desc*)0xc0000923) = make_gdt_desc((uint32_t*)&tss,
  tss_size - 1,TSS_ATTR_LOW,TSS_ATTR_HIGH);
  /* 加入了用户代码数据段 */
  *((struct gdt_desc*)0xc000092B) = make_gdt_desc((uint32_t*)0,
  0xfffff,GDT_CODE_ATTR_LOW_DPL3,TSS_ATTR_HIGH);
  *((struct gdt_desc*)0xc0000933) = make_gdt_desc((uint32_t*)0,
  0xfffff,GDT_DATA_ATTR_LOW_DPL3,TSS_ATTR_HIGH);
  /* 重装gdtr */
  uint64_t gdt_operand = ((8 * 7 - 1) | ((uint64_t)(uint32_t)0xc0000903 << 16));
  asm volatile("lgdt %0"::"m"(gdt_operand));
  asm volatile("ltr %w0"::"r"(SELECTOR_TSS));
  s_putstr("tss_init and ltr done done\n");

}



