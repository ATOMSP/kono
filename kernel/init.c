#include "../Include/lib/print.h"
#include "../Include/kernel/interrupt.h"


/**
 *  设备初始化
*/
void init_dev(void)
{
    s_putstr("start init dev...\n");
    int_init(); /* 中断初始化 */
}








