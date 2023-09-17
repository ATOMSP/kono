#include <kernel/timer.h>
#include <kernel/debug.h>
#include <lib/thread.h>
#include <kernel/interrupt.h>
#include <lib/io.h>

#define IRQ0_FREQUENCY  100
#define INPUT_FREQUENCY 1193180
#define COUNTER0_VALUE  INPUT_FREQUENCY / IRQ0_FREQUENCY
#define COUNTER0_PORT   0x40
#define COUNTER0_NO     0
#define COUNTER_MODE    2
#define READ_WRITE_LATCH  3
#define PIT_CONTROL_PORT  0x43

/**
 * 写时钟模式控制寄存器
*/
static void frequency_set(uint8_t counter_port,
                          uint8_t counter_no,
                          uint8_t rwl,
                          uint8_t counter_mode,
                          uint16_t counter_value){
  outb(PIT_CONTROL_PORT,(uint8_t)(counter_no << 6 | rwl << 4 | counter_mode << 1));  
  outb(counter_port,(uint8_t)counter_value);
  outb(counter_port,(uint8_t)counter_value >> 8);
}

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
  frequency_set(COUNTER0_PORT,COUNTER0_NO,READ_WRITE_LATCH,COUNTER_MODE,COUNTER0_VALUE);
  intr_handle_register(0x20,intr_timer_handler);
}













