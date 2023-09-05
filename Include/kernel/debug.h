#ifndef __KERNEL_DEBUG_H
#define __KERNEL_DEBUG_H

#ifdef __cplusplus
extern "C"{
#endif


void panic_spin(char* filename,int line,const char* func,const char* condition);

#define PANIC(...) panic_spin(__FILE__,__LINE__,__func__,__VA_ARGS__)

//定义断言
#ifdef NO_DEBUG
    #define ASSERT(condition) ((void)0)
#else
    #define ASSERT(condition) \
    if(condition){}else{\
        PANIC(#condition);\
    }
#endif


#ifdef __cplusplus
}
#endif

#endif








