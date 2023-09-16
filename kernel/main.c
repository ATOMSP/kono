#include <kernel/init.h>
#include <kernel/debug.h>
#include <kernel/interrupt.h>
#include <kernel/mm.h>
#include <lib/string.h>
#include <lib/print.h>
#include <lib/thread.h>

void thread(void* args);

int main()
{
    init_dev();
    thread_register("k_thread",31,thread,"Args ");
    while(1);
    return 0;                                                                                                                
}



void thread(void* args){
    char *param = args;
    int i = 10;
    while (1)
    {
        while (i--)
        {
            
        }
        i = 10;
        s_putstr(param);
    }
}































