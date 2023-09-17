[bits 32]

%define ERROR_CODE nop              ; 若cpu已经压入异常码 
%define ZERO       push 0           ; 则不进行操作，否则压入0

extern s_putstr                    ; 引入外部打印字符函数
extern Int_callback_table          ; 中断服务函数地址数组


SECTION .data
    int_msg db "Interrupt coming!",0x0a,0x00
    MSG_LEN equ $ - int_msg
global int_entry_table
int_entry_table:                        ; 组织中断服务表


%macro VECTOR 2                         ; 宏定义组织中断程序
SECTION .text
int%1_callback:
    %2                                  ; 对于cpu是否压栈错误码，不压的话就压0，压栈的话cpu这里就空转一下
    push ds                             ; 汇编跳到c，会破坏环境
    push es
    push fs
    push gs
    pushad
    ; push int_msg                      ; 测试使用过
    ; call s_putstr
    ; add  esp,4

    mov al,0x20
    out 0xa0,al                         ; 从片发送EOI
    out 0x20,al                         ; 主片发送EOI

    push %1                             ; 压入中断向量号
    call [Int_callback_table + %1 * 4]  ; 调用c中断服务函数
    add esp,4                           ; 跳过参数
    popad
    pop gs
    pop fs
    pop es
    pop ds
    add esp,4                           ; 不管有没有错误码，都要设置栈指针，因为人为对齐了
    iretd                               ; 32位中断返回

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
VECTOR 0x20,ZERO  ;时钟中断
VECTOR 0x21,ZERO  ;键盘中断
VECTOR 0x22,ZERO  ;级联使用
VECTOR 0x23,ZERO  ;串口2
VECTOR 0x24,ZERO  ;串口1
VECTOR 0x25,ZERO  ;并口2
VECTOR 0x26,ZERO  ;软盘
VECTOR 0x27,ZERO  ;并口1
VECTOR 0x28,ZERO  ;实时时钟
VECTOR 0x29,ZERO  ;重定向
VECTOR 0x2a,ZERO  ;保留
VECTOR 0x2b,ZERO  ;保留
VECTOR 0x2c,ZERO  ;ps/2鼠标
VECTOR 0x2d,ZERO  ;fpu浮点单元异常
VECTOR 0x2e,ZERO  ;硬盘
VECTOR 0x2f,ZERO  ;保留















