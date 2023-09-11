#include <lib/print.h>
#include <kernel/interrupt.h>
#include <kernel/mm.h>

/**
 *  设备初始化
*/
void init_dev(void)
{
    s_putstr("start init dev...\n");
    int_init(); /* 中断初始化 */
    init_mem();/* 内存初始化 */
}








