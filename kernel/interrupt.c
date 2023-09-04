#include "../Include/kernel/interrupt.h"
#include "../Include/kernel/macro.h"
#include "../Include/kernel/stdint.h"

#include "../Include/lib/print.h"
#include "../Include/lib/io.h"


// 中断门描述符结构体
struct Int_gate_desc
{
    uint16_t callback_offset_low_word;      // 中断服务程序在目标代码段的偏移量低16位
    uint16_t selector;                      // 中断服务程序目标代码段描述符选择子
    uint8_t  dcount;                        // 未使用，设置为0
    uint8_t  attribute;                     // P DPL S Type 四个控制字段
    uint16_t callback_offset_high_word;     // 中断服务程序在目标代码段的偏移量高16位
};

// interrupt.s中定义的所有的中断入口函数地址数组
extern Int_Handler_Typedef int_entry_table[IDT_DESC_NUM];
// 填充IDT
static void make_idt_desc(struct Int_gate_desc * desc,uint8_t attr,Int_Handler_Typedef callback);
// 中断描述符表（静态）
static struct Int_gate_desc IDT[IDT_DESC_NUM];


/**
 * 创建IDT
*/
static void make_idt_desc(struct Int_gate_desc * desc,uint8_t attr,Int_Handler_Typedef callback)
{
    if(desc){
        desc->attribute = attr;
        desc->dcount = 0;
        desc->selector = SELECTOR_K_CODE;
        desc->callback_offset_low_word = (uint32_t)callback & 0x0000ffff;
        desc->callback_offset_high_word = ((uint32_t)callback & 0xffff0000) >>16;        
    }
}

/**
 * 初始化中断描述符表
*/
static void init_idt_desc(void)
{
    s_putstr("Init idt desc...\n");
    int i;
    for (i = 0; i < IDT_DESC_NUM; i++)
    {
        make_idt_desc(&IDT[i],IDT_DESC_ATTR_DPL0,int_entry_table[i]);
    }
    s_putstr("Init idt desc OK!\n");
}

/**
 * 初始化8259A
*/
static void init_pic(void)
{
    s_putstr("init pic...\n");
    /*初始化主片*/                      //这里是8259A的初始化流程，没有什么要解释的
    outb(PIC_M_CTRL,0x11);
    outb(PIC_M_DATA,0x20);
    outb(PIC_M_DATA,0x04);
    outb(PIC_M_DATA,0x01);
    /*初始化从片*/
    outb(PIC_S_CTRL,0x11);
    outb(PIC_S_DATA,0x28);
    outb(PIC_S_DATA,0x02);
    outb(PIC_S_DATA,0x01);
    /* 打开主片IR0 目前只接受时钟的中断 */
    outb(PIC_M_DATA,0xfe);
    outb(PIC_S_DATA,0xff);    
    s_putstr("init pic OK!\n");
}

/**
 * 中断初始化
*/
void int_init(void)
{
    s_putstr("init_idt...\n");
    init_idt_desc();
    init_pic();
    uint64_t idtr = ((sizeof(IDT) - 1)|((uint64_t)((uint32_t)IDT << 16)));
    asm("lidt %0"::"m"(idtr));
    s_putstr("init_idt OK\n");
}
























