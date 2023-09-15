#include <lib/thread.h>
#include <kernel/mm.h>
#include <lib/string.h>

#define PG_SIZE 4096
#define STACK_MAGIC 0x20020417

/* 线程由此调用，此函数通过ret汇编指令进入 */
static void kernel_thread(thread_entry func,void* arg){
  func(arg);
}


/**
 * 初始化栈内环境结构
*/
void thread_create(struct task_struct * thread,thread_entry func,void* args){
  thread->self_stack -= sizeof(struct intr_stack);
  thread->self_stack -= sizeof(struct thread_stack);
  struct thread_stack * kthread_stack = (struct thread_stack*)thread->self_stack;
  kthread_stack->func_arg = args;
  kthread_stack->function = func;
  kthread_stack->unused_retaddr = NULL;
  /*将通过ret来使pc指向kernel_thread */
  kthread_stack->eip = kernel_thread;
  kthread_stack->ebp = kthread_stack->ebx = kthread_stack->edi = kthread_stack->esi = 0;
}

/**
 * 初始化线程PCB
*/
void init_thread(struct task_struct * thread,char* name,int prio){
  memset(thread,0,sizeof(*thread));
  strcpy(thread->name,name);
  thread->status = TASK_RUNNING;
  thread->priority = prio;
  //自己的线程栈初始化在页内高地址
  thread->self_stack = (uint32_t*)((uint32_t)thread + PG_SIZE);  
  thread->stack_magic = STACK_MAGIC;
}

/**
 * 注册线程
*/
struct task_struct* thread_register(char* name,
                                    int prio,
                                    thread_entry function,
                                    void* args){
  struct task_struct * thread = get_kernel_pages(1);
  init_thread(thread,name,prio);
  thread_create(thread,function,args);

  /* 准备好pcb，栈之后，开启kernel_thread */
  asm volatile("movl %0,%%esp;\
                pop %%ebp;\
                pop %%ebx;\
                pop %%edi;\
                pop %%esi;\
                ret"
                ::"g"(thread->self_stack):"memory");
  return thread;
}
                         



















