#ifndef __LIB_THREAD_H
#define __LIB_THREAD_H

#ifdef __cplusplus
extern "C"{
#endif

#include <kernel/stdint.h>

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




#ifdef __cplusplus
}
#endif



#endif




























