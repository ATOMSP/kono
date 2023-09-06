#include <kernel/init.h>
#include <kernel/debug.h>
#include <kernel/interrupt.h>
#include <lib/string.h>
#include <lib/print.h>



int main()
{
    init_dev();
    s_putstr("Hello SSH!\n");
    Int_Enable();
    while (1);
    return 0;                                                                                                                
}

































