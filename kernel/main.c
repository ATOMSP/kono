#include "../Include/lib/print.h"
#include "../Include/kernel/init.h"
#include "../Include/kernel/debug.h"





int main()
{
    init_dev();
    s_putstr("Makefile OK!");
    ASSERT(1==2);
    ASSERT(1==1);
    while (1);
    return 0;                                                                                                                
}

































