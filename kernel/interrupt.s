[bits 32]

%define ERROR_CODE nop              ; 若cpu已经压入异常码 
%define ZERO       push 0           ; 则不进行操作，否则压入0

extern s_putstr                    ; 引入外部打印字符函数

SECTION .data
    int_msg db "Interrupt coming!",0x0a,0x00
    MSG_LEN equ $ - int_msg
global int_entry_table
int_entry_table:                        ; 组织中断服务表


%macro VECTOR 2                         ; 宏定义组织中断程序
SECTION .text
int%1_callback:
    %2                                  ; 对于cpu是否压栈错误码，不压的话就压0，压栈的话cpu这里就空转一下
    push int_msg
    call s_putstr
    add  esp,4

    mov al,0x20
    out 0xa0,al                         ; 从片发送EOI
    out 0x20,al                         ; 主片发送EOI

    add esp,4                           ; 不管有没有错误码，都要设置栈指针，因为人为对齐了
    iret                                ; 中断返回
SECTION .data
    dd int%1_callback                   ; 存储中断入口程序地址32位
%endmacro

; 定义各种中断处理函数
; 1.处理器内部固定异常
VECTOR 0x00,ZERO
VECTOR 0x01,ZERO
VECTOR 0x02,ZERO
VECTOR 0x03,ZERO
VECTOR 0x04,ZERO
VECTOR 0x05,ZERO
VECTOR 0x06,ZERO
VECTOR 0x07,ZERO
VECTOR 0x08,ERROR_CODE
VECTOR 0x09,ZERO
VECTOR 0x0a,ERROR_CODE
VECTOR 0x0b,ERROR_CODE
VECTOR 0x0c,ERROR_CODE
VECTOR 0x0d,ERROR_CODE
VECTOR 0x0e,ERROR_CODE
VECTOR 0x0f,ZERO
VECTOR 0x10,ZERO
VECTOR 0x11,ERROR_CODE
VECTOR 0x12,ZERO
VECTOR 0x13,ZERO
; 2.Intel保留
VECTOR 0x14,ZERO
VECTOR 0x15,ZERO
VECTOR 0x16,ZERO
VECTOR 0x17,ZERO
VECTOR 0x18,ZERO
VECTOR 0x19,ZERO
VECTOR 0x1a,ZERO
VECTOR 0x1b,ZERO
VECTOR 0x1c,ZERO
VECTOR 0x1d,ZERO
VECTOR 0x1e,ZERO
VECTOR 0x1f,ZERO
; 用户使用
VECTOR 0x20,ZERO