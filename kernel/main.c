#include <kernel/init.h>
#include <kernel/debug.h>
#include <kernel/interrupt.h>
#include <kernel/mm.h>
#include <lib/string.h>
#include <lib/print.h>
#include <lib/thread.h>
#include <kernel/console.h>

void thread1(void* args);
void thread2(void* args);

int main()
{
    setCursor(0);
    s_putstr("Welcome to Kernel!\n");

    init_dev();
    thread_register("k_thread1",31,thread1,"Aais1");
    thread_register("k_thread2",8,thread2,"Aais2");
    Int_Enable();
    while(1){
        console_put_str("Main");
    }   
    return 0;                                                                                                                
}



void thread1(void* args){
    char *param = args;
    while (1)
    {
        console_put_str(param);
    }
}

void thread2(void* args){
    char *param = args;
    while (1)
    {
        console_put_str(param);
    }
}































