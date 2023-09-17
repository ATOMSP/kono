#ifndef __DRIVER_BUFF_QUEUE_H
#define __DRIVER_BUFF_QUEUE_H

#ifdef __cplusplus
extern "C"{
#endif

#include <kernel/stdint.h>
#include <lib/thread.h>
#include <lib/sync.h>

/*缓存区大小为129 - 1*/
#define BUFF_SIZE   129


/**
 * 环形队列
*/
struct buff_queue{
  /* 环形缓存区 */
  struct lock lock;
  struct task_struct * producer;
  struct task_struct * consumer;
  //缓存区
  char buf[BUFF_SIZE];
  //队列首尾
  int32_t head;
  int32_t tail;
};


/**
 * 环形缓存队列初始化
*/
void buff_queue_init(struct buff_queue* q);

/**
 * 生产者写入一个字符
*/
void buff_queue_putchar(struct buff_queue * q,char byte);

/**
 * 消费者读入一个字符
*/
char buff_queue_getchar(struct buff_queue* q);
/**
 * 判断队列是否已满
*/
int buff_queue_full(struct buff_queue* q);

/**
 * 判断队列是否为空
*/
int buff_queue_empty(struct buff_queue* q);



#ifdef __cplusplus
}
#endif

#endif