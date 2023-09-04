#include "../Include/lib/print.h"
#include "../Include/kernel/init.h"






int main()
{
    init_dev();
    asm volatile("sti");
    while (1);
    return 0;                                                                                                                
}

































