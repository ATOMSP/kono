#ifndef __KERNEL_INTERRUPT_H
#define __KERNEL_INTERRUPT_H



#ifdef __cplusplus
extern "C"{
#endif

// 中断服务入口函数地址（类型为void*）
typedef void* Int_Handler_Typedef;
// 8259A端口
#define PIC_M_CTRL  0x20        // 主片控制端口
#define PIC_M_DATA  0x21        // 主片数据端口
#define PIC_S_CTRL  0xa0        // 从片控制端口
#define PIC_S_DATA  0xa1        // 从片数据端口

//中断状态枚举
enum int_state{
    INT_OFF,
    INT_ON
};

/**
 * 中断初始化
*/
void int_init(void);
/**
 * 获取中断状态：INT_ON or INT_OFF
*/
enum int_state Int_Get_State(void);
/**
 * 设置中断状态：INT_ON or INT_OFF
*/
enum int_state Int_Set_State(enum int_state state);
/**
 * 禁止中断
*/
enum int_state Int_Disable(void);
/**
 * 开启中断
*/
enum int_state Int_Enable(void);



#ifdef __cplusplus
}
#endif









#endif






