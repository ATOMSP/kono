; ---------------------------------------打印核心函数------------------------
; 包含宏定义头文件
%include "Include/boot/macro.inc"


SELECTOR_VIDEO equ (0x0003 << 3)+TI_GDT+RPL_0 ;显卡段选择子


[bits 32]
SECTION .text

global s_putchar
; ---功能：打印单个字符（汇编实现）---
;
; -------------------------------
s_putchar:
    pushad                          ; 备份32寄存器环境
    mov ax,SELECTOR_VIDEO           ; 更新显卡选择子
    mov fs,ax
                                    ; 获取当前的光标位置
    mov dx,0x03d4                   ; 获取高八位
    mov al,0x0e
    out dx,al
    mov dx,0x03d5
    in  al,dx
    mov ah,al                       ; 获取低八位
    mov dx,0x03d4
    mov al,0x0f
    out dx,al
    mov dx,0x03d5
    in  al,dx   

    mov bx,ax
    mov ecx,[esp + 36]              ; 获取代打印字符
    
    cmp cl,0x0d                     ; 判断是否为特殊字符:回车
    jz  is_return
    cmp cl,0x0a                     ; 判断是否为特殊字符：换行
    jz  is_turn_line
    cmp cl,0x08
    jz  is_backspace                ; 判断是否为特殊字符：退格
    jmp put_char



put_char:


is_backspace:

is_return:
    add bx,80
    cmp bx,2000
is_turn_line:
    jl set_cursor
set_cursor:

















