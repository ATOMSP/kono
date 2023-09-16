#include <kernel/timer.h>
#include <kernel/debug.h>
#include <lib/thread.h>
#include <kernel/interrupt.h>


//内核中断开启以来总的滴答数
uint32_t ticks = 0;

/**
 * 时钟中断回调函数
*/
static void intr_timer_handler(void)
{
  struct task_struct * cur = get_cur_thread_pcb();
  //检查是否栈溢出
  ASSERT(cur->stack_magic == STACK_MAGIC);
  cur->elapsed_ticks ++; //相当于时间戳
  ticks++;
  if(cur->ticks == 0){
    /* 启动调度器 */
    schedule();
  }else{
    cur->ticks --;
  }
} 

/**
 * 时钟初始化
*/
void init_timer(void)
{
  intr_handle_register(0x20,intr_timer_handler);
}













