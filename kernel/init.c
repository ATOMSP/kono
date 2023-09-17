#include <lib/print.h>
#include <kernel/interrupt.h>
#include <kernel/mm.h>
#include <kernel/timer.h>
#include <lib/thread.h>
#include <kernel/console.h>
#include <driver/keyboard.h>
/**
 *  设备初始化
*/
void init_dev(void)
{
    s_putstr("start init dev...\n");
    int_init(); /* 中断初始化 */
    init_timer();/* 时钟初始化 */
    init_mem();/* 内存初始化 */
    init_thread();/*线程初始化 */
    console_init();/* 终端初始化 */
    keyboard_init();/* 键盘初始化 */
}








