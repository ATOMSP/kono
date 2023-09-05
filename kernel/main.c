#include "../Include/lib/print.h"
#include "../Include/kernel/init.h"
#include "../Include/kernel/debug.h"
#include "../Include/lib/string.h"
#include "../Include/kernel/interrupt.h"



int main()
{
    init_dev();
    Int_Enable();
    while (1);
    return 0;                                                                                                                
}

































