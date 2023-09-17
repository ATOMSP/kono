#include <kernel/init.h>
#include <kernel/debug.h>
#include <kernel/interrupt.h>
#include <kernel/mm.h>
#include <lib/string.h>
#include <lib/print.h>
#include <lib/thread.h>
#include <kernel/console.h>

#include <driver/keyboard.h>
#include <driver/buff_queue.h>

void thread1(void* args);
void thread2(void* args);

int main()
{
    setCursor(0);
    s_putstr("Welcome to Kernel!\n");

    init_dev();
    thread_register("k_thread1",31,thread1,"A_");
    thread_register("k_thread2",31,thread2,"B_");
    Int_Enable();
    while(1);
    return 0;                                                                                                                
}



void thread1(void* args){
    while (1)
    {
        enum int_state old_state = Int_Disable();
        if(!buff_queue_empty(&keyboard_buff)){
          console_put_str((char*)args);
          char byte = buff_queue_getchar(&keyboard_buff);
          console_put_char(byte);           
        }
        Int_Set_State(old_state);
    }
}

void thread2(void* args){
    while (1)
    {
        enum int_state old_state = Int_Disable();
        if(!buff_queue_empty(&keyboard_buff)){
          console_put_str((char*)args);
          char byte = buff_queue_getchar(&keyboard_buff);
          console_put_char(byte);           
        }
        Int_Set_State(old_state);
    }
}































