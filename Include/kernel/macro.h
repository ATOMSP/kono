#ifndef __KERNEL_MACRO_H
#define __KERNEL_MACRO_H



#ifdef __cplusplus
extern "C"{
#endif

// IDT中相关宏定义（要写入中断描述符中的参数）
#define RPL_0   0                           // 中断描述符特权级
#define RPL_1   1
#define RPL_2   2
#define RPL_3   3
#define TI_GDT  0
#define TI_LDT  1
#define SELECTOR_K_CODE         (0x0001 << 3) + (TI_GDT << 2) + RPL_0           // 内核代码段
#define SELECTOR_K_DATA         (0x0002 << 3) + (TI_GDT << 2) + RPL_0           // 内核数据段
#define SELECTOR_K_STACK        SELECTOR_K_DATA                      // 内核堆栈段
#define SELECTOR_K_FS           (0x0003 << 3) + (TI_GDT << 2) + RPL_0           // 内核显卡段

// IDT描述符
#define IDT_DESC_P              1
#define IDT_DESC_DPL0           0
#define IDT_DESC_DPL3           3
#define IDT_DESC_32_TYPE        0xe
#define IDT_DESC_16_TYPE        0x6
// IDT的attr参数
#define IDT_DESC_ATTR_DPL0      ((IDT_DESC_P << 7) + (IDT_DESC_DPL0 << 5) + IDT_DESC_32_TYPE)
#define IDT_DESC_ATTR_DPL3      ((IDT_DESC_P << 7) + (IDT_DESC_DPL3 << 5) + IDT_DESC_32_TYPE)

// 中断个数
#define IDT_DESC_NUM            0x21 


#ifdef __cplusplus
}
#endif









#endif






