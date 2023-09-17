#ifndef __LIB_SYNC_H
#define __LIB_SYNC_H

#ifdef __cplusplus
extern "C"{
#endif

#include <kernel/stdint.h>
#include <lib/thread.h>

// 二元信号量
struct semaphore { 
  uint8_t value; 
  struct list  waiters;
};

// 锁数据结构
struct lock { 
  struct task_struct * holder; //锁的持有线程
  struct semaphore semaphore;  //二元信号量
  uint32_t holder_repeat_nr;   //持有者重复申请锁的次数
};



/**
 * 锁初始化
*/
void lock_init(struct lock * lock);
/**
 * 获取锁,并上锁
*/
void lock_acquire(struct lock * lock);
/**
 * 释放锁
*/
void lock_release(struct lock * lock);



#ifdef __cplusplus
}
#endif



#endif