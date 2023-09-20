#ifndef __KERNEL_TSS_H
#define __KERNEL_TSS_H

#ifdef __cplusplus
extern "C"{
#endif


/**
 * 在GDT中创建tss并重新加载GDT
*/
void tss_init(void);


#ifdef __cplusplus
}
#endif



#endif



