#ifndef __KERNEL_MACRO_H
#define __KERNEL_MACRO_H



#ifdef __cplusplus
extern "C"{
#endif


// GDT描述符属性
#define DESC_G_4K   1
#define DESC_D_32   1
#define DESC_L      0
#define DESC_AVL    0
#define DESC_P      1
#define DESC_DPL_0  0
#define DESC_DPL_1  1
#define DESC_DPL_2  2
#define DESC_DPL_3  3

#define DESC_S_CODE 1
#define DESC_S_DATA 1
#define DESC_S_SYS  0
#define DESC_TYPE_CODE  8 //x = 1 c = 0 r = 0 a = 0
#define DESC_TYPE_DATA  2//x = 0 e = 0 w = 1 a = 0
//描述符属性为TSS
#define DESC_TYPE_TSS   9 // b = 0 表示tss不嵌套

// IDT中相关宏定义（要写入中断描述符中的参数）
#define RPL_0   0                           // 中断描述符特权级
#define RPL_1   1
#define RPL_2   2
#define RPL_3   3
#define TI_GDT  0
#define TI_LDT  1
#define SELECTOR_K_CODE         ((0x0001 << 3) + (TI_GDT << 2) + RPL_0)           // 内核代码段
#define SELECTOR_K_DATA         ((0x0002 << 3) + (TI_GDT << 2) + RPL_0)           // 内核数据段
#define SELECTOR_K_STACK        SELECTOR_K_DATA                      // 内核堆栈段
#define SELECTOR_K_FS           ((0x0003 << 3) + (TI_GDT << 2) + RPL_0)           // 内核显卡段
//用户段
#define SELECTOR_U_CODE         ((0x0005 << 3) + (TI_GDT << 2) + RPL_3)         
#define SELECTOR_U_DATA         ((0x0006 << 3) + (TI_GDT << 2) + RPL_3)
#define SELECTOR_U_STACK        SELECTOR_U_DATA
//GDT属性高位的设置
#define GDT_ATTR_HIGH           ((DESC_G_4K << 7) + (DESC_D_32 << 6) + (DESC_L << 5) + (DESC_AVL << 4))
//GDT属性低位的用户级权限设置
#define GDT_CODE_ATTR_LOW_DPL3 \
(\
  (DESC_P << 7) +\
  (DESC_DPL_3 << 5) +\
  (DESC_S_CODE << 4) +\
  (DESC_TYPE_CODE)\
)
#define GDT_DATA_ATTR_LOW_DPL3 \
(\
  (DESC_P << 7) +\
  (DESC_DPL_3 << 5) +\
  (DESC_S_CODE << 4) +\
  (DESC_TYPE_DATA)\
)

/*-----------TSS描述符属性--------------*/
#define TSS_DESC_D  0
#define TSS_ATTR_HIGH \
(\
  (DESC_G_4K << 7) +\
  (TSS_DESC_D << 6) +\
  (DESC_L << 5) +\
  (DESC_AVL << 4) + 0x0\
)
#define TSS_ATTR_LOW \
(\
  (DESC_P << 7) +\
  (DESC_DPL_0 << 5) +\
  (DESC_S_SYS << 4) +\
  (DESC_TYPE_TSS)\
)
//TSS描述符选择子
#define SELECTOR_TSS  ((0x0004 << 3) + (TI_GDT << 2) + RPL_0)
//这里定义GDT结构纯粹是为了加用C语言加TSS到GDT表中
struct gdt_desc{
  uint16_t limit_low_word;
  uint16_t base_low_word;
  uint8_t  base_mid_byte;
  uint8_t  attr_low_byte;
  uint8_t  limit_high_attr_high;
  uint8_t  base_high_byte;
};




// IDT描述符
#define IDT_DESC_P              1
#define IDT_DESC_DPL0           0
#define IDT_DESC_DPL3           3
#define IDT_DESC_32_TYPE        0xe
#define IDT_DESC_16_TYPE        0x6
// IDT的attr参数
#define IDT_DESC_ATTR_DPL0      ((IDT_DESC_P << 7) + (IDT_DESC_DPL0 << 5) + IDT_DESC_32_TYPE)
#define IDT_DESC_ATTR_DPL3      ((IDT_DESC_P << 7) + (IDT_DESC_DPL3 << 5) + IDT_DESC_32_TYPE)

// 中断向量号个数
#define IDT_DESC_NUM            0x30 


#ifdef __cplusplus
}
#endif









#endif






