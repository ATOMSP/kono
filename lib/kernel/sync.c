#include <lib/sync.h>
#include <lib/string.h>
#include <kernel/interrupt.h>
#include <kernel/debug.h>

/**
 * 信号量初始化
*/
void sema_init(struct semaphore * sema,uint8_t val)
{
  sema->value = val;
  list_init(&sema->waiters);
}


/**
 * 锁初始化
*/
void lock_init(struct lock * lock)
{
  lock->holder = NULL;
  lock->holder_repeat_nr = 0;
  sema_init(&lock->semaphore,1);
}

/**
 * 信号量down操作(线程访问资源上锁)
*/
void sema_down(struct semaphore* psema) {
  enum int_state old_status = Int_Disable(); 
  /* 信号量被其他线程持有 */
  while(psema->value == 0){
    //当前线程不应该在等待队列中
    ASSERT(!elem_find(&psema->waiters, &get_cur_thread_pcb()->general_tag)); 
    if(elem_find(&psema->waiters, &get_cur_thread_pcb()->general_tag)){
      PANIC("sema_down : thread blocked has been in waiters list\n"); 
    }
    //线程加入锁的持有者线程的等待队列中，并阻塞自己
    list_append(&psema->waiters, &get_cur_thread_pcb()->general_tag); 
    thread_block(TASK_BLOCKED);
  }
  //当value不为0时，线程可以获得锁
  psema->value--; 
  ASSERT(psema->value == 0); 
  Int_Set_State(old_status); 
}


/**
 * 信号量up操作
*/
void sema_up(struct semaphore* psema) {
  enum int_state old_status = Int_Disable(); 
  //确保信号量值为0
  ASSERT(psema->value == 0);
  if(!list_empty(&psema->waiters)){
    //取出一个阻塞链表，并将其挂到就绪链表
    struct list_elem * elem = list_pop(&psema->waiters);
    struct task_struct * temp = elem2entry(struct task_struct,general_tag,elem);
    //唤醒阻塞线程
    thread_unblock(temp);
  }
  //没有阻塞线程在等待队列，就直接解锁
  psema->value++;
  ASSERT(psema->value == 1);  
  Int_Set_State(old_status); 
}

/**
 * 获取锁,并上锁
*/
void lock_acquire(struct lock * lock)
{
  if(lock->holder != get_cur_thread_pcb()){
    //上锁
    sema_down(&lock->semaphore);
    lock->holder = get_cur_thread_pcb();
    ASSERT(lock->holder_repeat_nr == 0);
    lock->holder_repeat_nr = 1;
  }else{
    //记录未释放锁的线程，获取锁的次数
    lock->holder_repeat_nr ++;
  }
}

/**
 * 释放锁
*/
void lock_release(struct lock * lock)
{
  ASSERT(lock->holder == get_cur_thread_pcb()); 
  //如果持有者多次获取锁
  if (lock->holder_repeat_nr > 1) { 
    lock->holder_repeat_nr--; 
    return; 
  } 
  //持有者已经使用完毕
  ASSERT(lock->holder_repeat_nr == 1); 
  lock->holder = NULL; 
  lock->holder_repeat_nr = 0; 
  //对信号量解锁
  sema_up(&lock->semaphore);
}
