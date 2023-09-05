#include "../Include/kernel/debug.h"
#include "../Include/lib/print.h"
#include "../Include/kernel/interrupt.h"


/**
 * 断言失败函数
*/
void panic_spin(char* filename,int line,const char* func,const char* condition)
{
    Int_Disable(); //关闭中断，禁止调度
    s_putstr("\n****Error****\n");
    s_putstr("filename:");s_putstr((const char*)filename);s_putchar('\n');
    s_putstr("line: 0x");s_putnum(line);s_putchar('\n');
    s_putstr("function:");s_putstr((const char*)func);s_putchar('\n');
    s_putstr("condition:");s_putstr((const char*)condition);s_putchar('\n');
    s_putstr("****Error****\n");
    while (1);
}

