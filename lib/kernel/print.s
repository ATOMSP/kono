; ---------------------------------------打印核心函数------------------------
; 包含宏定义头文件
%include "Include/boot/macro.inc"


SELECTOR_VIDEO equ (0x0003 << 3)+TI_GDT+RPL_0 ;显卡段选择子


[bits 32]
SECTION .text

global s_putchar

; ---功能：打印单个字符（汇编实现）---
;   输入：单个字符（c语言调用）
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

    mov bx,ax                       ; 当前光标的位置
    mov ecx,[esp + 36]              ; 获取代打印字符
    
    cmp cl,0x0d                     ; 判断是否为特殊字符:回车
    jz  is_return
    cmp cl,0x0a                     ; 判断是否为特殊字符：换行
    jz  is_turn_line
    cmp cl,0x08
    jz  is_backspace                ; 判断是否为特殊字符：退格
    jmp put_char

is_backspace:                       ; 处理退格：不显示字符，光标回退，前一个字符用空格代替
    dec bx                          ; 这段处理需要手推一遍，比较绕
    shl bx,1
    mov byte [fs:bx],0x00
    inc bx
    mov byte [fs:bx],0x07
    shr bx,1
    jmp set_cursor
put_char:
    shl bx,1
    mov byte [fs:bx],cl
    inc bx
    mov byte [fs:bx],0x07
    shr bx,1
    inc bx
    cmp bx,2000                     ; 80*25一个2000，表示光标位置不能大于等于2000
    jl  set_cursor
is_turn_line:                       ; 换行
is_return:                          ; 回车
    sub dx,dx                       
    mov ax,bx
    mov si,80
    div si
    sub bx,dx                       ; 此时光标位置bx在本行的行首
is_return_end:     
    add bx,80                       ; 蒋光标值移动至下一行行首
    cmp bx,2000                         
is_turn_line_end:
    jl set_cursor
; 屏幕满了需要滚动屏幕
roll_screen:
    cld
    mov ecx,960                     ; 搬运80*24=1920个字节，一次搬运4个字节。所以搬运960次
    mov esi,0xc00b80a0              ; 第一行首地址
    mov edi,0xc00b8000              ; 第0行首地址
    rep movsd                       ; 四字节搬运
    ; 填充最后一行为0
    mov ebx,3840
    mov ecx,80
clear_last_line:
    mov word [fs:ebx],0x0700
    add ebx,2
    loop clear_last_line
    mov bx,1920                     ; 将光标位置移动到最后一行的行首
set_cursor:
    mov dx,0x03d4                   ; 设置高八位
    mov al,0x0e
    out dx,al
    mov dx,0x03d5
    mov al,bh
    out dx,al

    mov dx,0x03d4                   ; 设置低八位
    mov al,0x0f
    out dx,al
    mov dx,0x03d5
    mov al,bl
    out dx,al
put_char_done:
    popad
    ret

; ---功能：打印字符串（汇编实现）-----
;   输入：字符串地址（c语言调用）
; -------------------------------
global s_putstr

s_putstr:   
    push ebx                        ; 压栈ebx,ecx
    push ecx                        
    sub  ecx,ecx                    
    mov  ebx,[esp + 12]             ; 获取参数，即字符串的地址
goon:
    mov  cl,[ebx]                   ; 判断字符串是否为结束位\0
    cmp  cl,0
    jz   is_str_end
    push ecx
    call s_putchar                  ; 调用打印字符操作
    add  esp,4
    inc  ebx
    jmp  goon
is_str_end:
    pop  ecx
    pop  ebx
    ret

; ---功能：打印整形数字（汇编实现）---
;   输入：数字（c语言调用）
; -------------------------------
global s_putnum

s_putnum:
    pushad                          ; 压栈所有通用寄存器
    mov ebp,esp
    mov eax,[ebp + 36]
    mov edx,eax
    mov edi,7
    mov ecx,8
    mov ebx,putnum_buff
loop_base_4bits:
    and edx,0x0000000f
    cmp edx,9
    jg  is_a2f
    add edx,'0'
    jmp to_store
is_a2f:
    sub edx,10
    add edx,'A'
to_store:
    mov [ebx + edi],dl
    dec edi
    shr eax,4
    mov edx,eax
    loop loop_base_4bits
ready_to_print:
    inc edi
skip_prefix_0:
    cmp edi,8
    je  full_0
go_on_skip:
    mov cl,[ebx + edi]
    inc edi
    cmp cl,'0'
    je  skip_prefix_0
    dec edi
    jmp put_each_char
full_0:
    mov cl,'0'
put_each_char:
    push ecx
    call s_putchar
    add  esp,4
    inc  edi
    mov cl,[ebx + edi]
    cmp  edi,8
    jl  put_each_char
    popad
    ret
; -------------------------------------------------数据区域----------------------------------------
SECTION .data
    putnum_buff dq 0                ; 定义缓存










