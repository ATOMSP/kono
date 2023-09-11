#include <kernel/init.h>
#include <kernel/debug.h>
#include <kernel/interrupt.h>
#include <kernel/mm.h>
#include <lib/string.h>
#include <lib/print.h>


int main()
{
    init_dev();
    Int_Disable();
    void* addr = get_kernel_pages(3);
    s_putstr("\n get_kernel_pages start addr is 0x");
    s_putnum((int)addr);
    s_putchar('\n');
    while (1);
    return 0;                                                                                                                
}

































