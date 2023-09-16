#ifndef __LIB_THREAD_H
#define __LIB_THREAD_H

#ifdef __cplusplus
extern "C"{
#endif

#include <kernel/stdint.h>
#include <kernel/list.h>

#define STACK_MAGIC 0x20020417
//线程入口函数
typedef void(*thread_entry)(void*);
//线程状态
enum task_state{
  TASK_RUNNING,
  TASK_READY,
  TASK_HANGING,
  TASK_WAITING,
  TASK_DIED,
  TASK_BLOCKED
};
//中断栈：发生中断时，保护线程上下文，需要按照此结构依次压栈
struct intr_stack{
  uint32_t vec_id;// 发生中断时，压入的中断向量号
  uint32_t edi;  
  uint32_t esi;
  uint32_t ebp;
  uint32_t esp_dummy;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;
  uint32_t gs;
  uint32_t fs;
  uint32_t es;
  uint32_t ds;
  //一下为cpu由低特权级到高特权级压入
  uint32_t err_code;
  void (*eip)(void);
  uint32_t cs;
  uint32_t eflags;
  void* esp;
  uint32_t ss;
};

//线程栈thread_stack
struct thread_stack{
  uint32_t ebp;
  uint32_t ebx;
  uint32_t edi;
  uint32_t esi;
  /* 线程启动函数，多个线程时是代表转换后的函数入口 */
  void (*eip)(thread_entry func,void * func_arg);
  /* 提供同c语言一致的函数调用栈结构 */
  void (*unused_retaddr);
  thread_entry function;
  void* func_arg;
};


/* 进程或线程的PCB */
struct task_struct{
  uint32_t * self_stack;
  enum task_state status;
  uint8_t priority;
  char name[16];
  uint8_t ticks; // 线程单次运行时钟滴答数
  uint32_t elapsed_ticks;//线程自首次运行后的累计滴答数

  struct list_elem general_tag; //general_tag 的作用是用于线程在一般的队列中的结点
  struct list_elem all_list_tag; //all list tag 的作用是用于线程队列 thread_all list 中的结点
  uint32_t* pgdir; //线程所在页表虚拟地址 

  uint32_t stack_magic;
};

/**
 * 注册线程
*/
struct task_struct* thread_register(char* name,
                                    int prio,
                                    thread_entry function,
                                    void* args);
struct task_struct* get_cur_thread_pcb(void);

/**
 * 任务调度器
*/
void schedule(void);


#ifdef __cplusplus
}
#endif



#endif




























