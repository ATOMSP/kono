#include <kernel/interrupt.h>
#include <kernel/macro.h>
#include <kernel/stdint.h>
#include <lib/print.h>
#include <lib/io.h>


// interrupt.s中定义的所有的中断入口函数地址数组
extern Int_Handler_Typedef int_entry_table[IDT_DESC_NUM];

// 中断门描述符结构体
struct Int_gate_desc
{
    uint16_t callback_offset_low_word;      // 中断服务程序在目标代码段的偏移量低16位
    uint16_t selector;                      // 中断服务程序目标代码段描述符选择子
    uint8_t  dcount;                        // 未使用，设置为0
    uint8_t  attribute;                     // P DPL S Type 四个控制字段
    uint16_t callback_offset_high_word;     // 中断服务程序在目标代码段的偏移量高16位
};

char* IntAbnormal_name[IDT_DESC_NUM];// 保存异常名称表
Int_Handler_Typedef Int_callback_table[IDT_DESC_NUM];//中断服务程序入口
// 填充IDT
static void make_idt_desc(struct Int_gate_desc * desc,uint8_t attr,Int_Handler_Typedef callback);
// 中断描述符表（静态）
static struct Int_gate_desc IDT[IDT_DESC_NUM];

/**
 * 通用异常处理服务函数：参数为中断向量号
*/
static void Int_general_callback(uint8_t vec)
{
    if(vec == 0x27 || vec == 0x2f){
        return;
    }
    s_putstr("Interrupt Vector is:0x");
    s_putnum(vec);
    s_putchar('\n');
}

/**
 * 中断服务程序注册及名称注册（前20号）
*/
static void Int_register(void)
{
    int i;
    for (i = 0; i < IDT_DESC_NUM; i++){
        Int_callback_table[i] = Int_general_callback;
        IntAbnormal_name[i] = "Unknown";
    }
    IntAbnormal_name[0] = "#DE Divide Error:0N";
    IntAbnormal_name[1] = "#DB Debug 1N";
    IntAbnormal_name[2] = "/NMI Interrupt 2N";
    IntAbnormal_name[3] = "#BP Breakpoint 3N";
    IntAbnormal_name[4] = "#OF Overflow 4N";
    IntAbnormal_name[5] = "#BR Bound Range Exceeded 5N";
    IntAbnormal_name[6] = "#UD Invalid Opcode 6N";
    IntAbnormal_name[7] = "#NM Device Not Available 7N";
    IntAbnormal_name[8] = "#DF Double Fault 8Y(0)";
    IntAbnormal_name[9] = "#MF Coprocessor Segment Overrun 9N";
    IntAbnormal_name[10] = "#TS Invalid TSS 10Y";
    IntAbnormal_name[11] = "#NP Segment Not Present 11Y"; 
    IntAbnormal_name[12] = "#SS Stack Segment Fault 12Y";
    IntAbnormal_name[13] = "#GP General Protection 13Y";
    IntAbnormal_name[14] = "#PF Page Fault 14Y";
    //IntAbnormal_name[15] = "";
    IntAbnormal_name[16] = "#MF Float-point Error 16N";
    IntAbnormal_name[17] = "#AC Alignment Check 17Y(0)";
    IntAbnormal_name[18] = "#MC Machine Check 18N";
    IntAbnormal_name[19] = "#XM SIMD Float-point Exception 19N";
}


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
    init_idt_desc();//初始化IDT(创建表，填充内容)
    Int_register();// 注册c中断服务函数，汇编中的那个中断服务函数地址只是一个入口，最终调用的还是Int_callback中的函数
    init_pic();
    uint64_t idtr = ((sizeof(IDT) - 1)|((uint64_t)((uint32_t)IDT << 16)));
    asm("lidt %0"::"m"(idtr));
    s_putstr("init_idt OK\n");
}

/*------------------------中断相关操作----------------------------*/
#define EFLAGS_IF 0x00000200
#define GET_EFLAGS(EFLAGS_VAR) asm volatile("pushfl; \
                                             popl %0" \
                                             :"=g"(EFLAGS_VAR));

/**
 * 开启中断
*/
enum int_state Int_Enable(void)
{
    enum int_state pre;
    if(INT_ON == Int_Get_State()){
        pre = INT_ON;
    }else{
        pre = INT_OFF;
        asm volatile("sti");
    }
    return pre;
}

/**
 * 禁止中断
*/
enum int_state Int_Disable(void)
{
    enum int_state pre;
    if(INT_OFF == Int_Get_State()){
        pre = INT_OFF;
    }else{
        pre = INT_ON;
        asm volatile("cli":::"memory"); //关闭中断用到内存操作，因为要修改if位
    }
    return pre;    
}

/**
 * 设置中断状态：INT_ON or INT_OFF
*/
enum int_state Int_Set_State(enum int_state state)
{
    return (state & INT_ON) ? Int_Enable() : Int_Disable();
}



/**
 * 获取中断状态：INT_ON or INT_OFF
*/
enum int_state Int_Get_State(void)
{
    uint32_t cur = 0;
    GET_EFLAGS(cur);
    return (cur & EFLAGS_IF) ? INT_ON : INT_OFF;
}

















