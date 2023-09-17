#include <driver/buff_queue.h>
#include <lib/string.h>
#include <kernel/debug.h>
#include <kernel/interrupt.h>

/**
 * 环形缓存队列初始化
*/
void buff_queue_init(struct buff_queue* q)
{
  memset(q->buf,0,BUFF_SIZE);
  lock_init(&q->lock);
  q->producer = q->consumer = NULL;
  q->tail = q->head = 0;
}

/**
 * 返回pos在缓存区的下一个偏移量
*/
static int32_t next_pos(int32_t pos){
  return (pos + 1) % BUFF_SIZE;
}

/**
 * 判断队列是否已满
*/
int buff_queue_full(struct buff_queue* q)
{
  /* 断言中断已经关闭 */
  ASSERT(Int_Get_State() == INT_OFF);
  return (next_pos(q->head) == q->tail);  
}

/**
 * 判断队列是否为空
*/
int buff_queue_empty(struct buff_queue* q)
{
   /* 断言中断已经关闭 */
  ASSERT(Int_Get_State() == INT_OFF); 
  return (q->tail == q->head);
}

/**
 * 使生产者与消费者在缓存区上等待
*/
static void buff_queue_wait(struct task_struct** waiter){
  ASSERT(*waiter == NULL && waiter != NULL);
  *waiter = get_cur_thread_pcb();
  thread_block(TASK_BLOCKED);
}
/**
 * 唤醒等待线程
*/
static void wakeup(struct task_struct** waiter){
  ASSERT(*waiter != NULL);
  thread_unblock(*waiter);
  *waiter = NULL;
}
/**
 * 消费者读入一个字符
*/
char buff_queue_getchar(struct buff_queue* q)
{
  ASSERT(Int_Get_State() == INT_OFF);
  while(buff_queue_empty(q)){
    lock_acquire(&q->lock);
    buff_queue_wait(&q->consumer);
    lock_release(&q->lock);
  }
  char byte = q->buf[q->tail];
  q->tail = next_pos(q->tail);
  if(q->producer != NULL){
    wakeup(&q->producer);
  }
  return byte;
}

/**
 * 生产者写入一个字符
*/
void buff_queue_putchar(struct buff_queue * q,char byte){
  ASSERT(Int_Get_State() == INT_OFF);
  while(buff_queue_full(q)){
    lock_acquire(&q->lock);
    buff_queue_wait(&q->producer);
    lock_release(&q->lock);
  }
  q->buf[q->head] = byte;
  q->head = next_pos(q->head);
  if(q->consumer != NULL){
    wakeup(&q->consumer);
  }
}




