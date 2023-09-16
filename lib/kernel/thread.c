#include <lib/thread.h>
#include <kernel/mm.h>
#include <kernel/interrupt.h>
#include <kernel/debug.h>
#include <lib/string.h>
#include <lib/print.h>


#define PG_SIZE 4096


/* 定义队列，主线程*/
struct task_struct * main_thread; // 主线程PCB
struct list thread_ready_list;    // 就绪队列
struct list thread_all_list;      // 任务队列
static struct list_elem * thread_tag; //保存队列中的线程结点

//外部实现任务切换函数
extern void switch_to(struct task_struct * cur,struct task_struct * next);

/**
 * 通过线程栈指针间接获取pcb地址
*/
struct task_struct* get_cur_thread_pcb(void){
  uint32_t esp;
  asm volatile("mov %%esp,%0":"=g"(esp));
  return (struct task_struct *)(esp & (0xfffff000));
}


/* 线程由此调用，此函数通过ret汇编指令进入 */
static void kernel_thread(thread_entry func,void* arg){
  //确保中断开启
  Int_Enable();  
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
  //主线程需要一直运行
  if(thread == main_thread){
    thread->status = TASK_RUNNING;
  }else{
    thread->status = TASK_READY;
  }
  thread->priority = prio;
  thread->ticks = prio;
  thread->elapsed_ticks = 0;
  //线程共享进程资源，故没有自己的页表
  thread->pgdir = NULL;
  //自己的线程栈初始化在页内高地址
  thread->self_stack = (uint32_t*)((uint32_t)thread + PG_SIZE);  
  thread->stack_magic = STACK_MAGIC;
}

/**
 * 注册线程函数
*/
struct task_struct* thread_register(char* name,
                                    int prio,
                                    thread_entry function,
                                    void* args){
  struct task_struct * thread = get_kernel_pages(1);
  init_thread(thread,name,prio);
  thread_create(thread,function,args);
  /* 确保该任务之前不在队列 */
  ASSERT(elem_find(&thread_ready_list,&thread->general_tag) == 0);
  /* 将任务添加至就绪队列中 */
  list_append(&thread_ready_list,&thread->general_tag); 
  /* 确保该任务之前不在全部队列 */
  ASSERT(elem_find(&thread_all_list,&thread->all_list_tag) == 0);
  /* 将任务添加至全部队列中 */
  list_append(&thread_all_list,&thread->all_list_tag); 
  /* 准备好pcb，栈之后，开启kernel_thread */
  // asm volatile("movl %0,%%esp;\
  //               pop %%ebp;\
  //               pop %%ebx;\
  //               pop %%edi;\
  //               pop %%esi;\
  //               ret"
  //               ::"g"(thread->self_stack):"memory");
  return thread;
}

/**
 * 设置主线程PCB
*/
static void make_main_thread(void)
{
  //内核主线程的pcb在setup.s中已经预留,并且栈已经设置为页高地址处
  //此时栈顶指针一直初始化为0xc009f000
  main_thread = get_cur_thread_pcb();
  s_putstr("main thread pcb addr is 0x");
  s_putnum((uint32_t)main_thread);
  s_putchar('\n');
  init_thread(main_thread,"main",31);
  // main线程需要一直运行，故不需要加入就绪列表
  ASSERT(elem_find(&thread_all_list,&main_thread->all_list_tag) == 0);
  list_append(&thread_all_list,&main_thread->all_list_tag);

}
/**
 * 任务调度器
*/
void schedule(void)
{
  //确保中断关闭
  ASSERT(Int_Get_State() == INT_OFF);
  //获取当前的线程pcb
  struct task_struct * cur = get_cur_thread_pcb();
  ASSERT(cur->ticks == 0);
  //判断是否为时间片结束的调度
  if(cur->status == TASK_RUNNING){
    ASSERT(!elem_find(&thread_ready_list,&cur->general_tag));
    //恢复时间片
    cur->ticks = cur->priority;
    //状态转移为就绪
    cur->status = TASK_READY;
    //挂到就绪列表后
    list_append(&thread_ready_list,&cur->general_tag);
  }
  //检查列表是否为空  
  ASSERT(list_empty(&thread_ready_list) == 0);
  thread_tag = NULL;
  thread_tag = list_pop(&thread_ready_list);
  struct task_struct * next = elem2entry(struct task_struct,general_tag,thread_tag);
  next->status = TASK_RUNNING;
  switch_to(cur,next);
}

















