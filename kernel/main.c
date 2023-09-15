#include <kernel/init.h>
#include <kernel/debug.h>
#include <kernel/interrupt.h>
#include <kernel/mm.h>
#include <lib/string.h>
#include <lib/print.h>
#include <lib/thread.h>


void thread(void* args){
    char *param = args;
    while (1)
    {
        s_putstr(param);
    }
}


int main()
{
    init_dev();
    Int_Disable();
    thread_register("k_thread",31,thread,"Args");
    return 0;                                                                                                                
}

































