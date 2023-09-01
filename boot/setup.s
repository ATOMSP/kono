; 包含宏定义头文件
%include "Include/boot/macro.inc"
; 告诉汇编器将下面的程序编址起始为0x900
SECTION SETUP vstart=SETUP_BASE_ADDR
STACK_TOP_ADDR equ SETUP_BASE_ADDR

    ; 跳转至setup入口进行启动保护模式的准备
    jmp setup_start
    ; 定义GDT相关数据结构
    GDT_BASE:   dd 0x00000000
                dd 0x00000000       ; 第0个段描述符（不可用）

    CODE_DESC:  dd 0x0000ffff  
                dd DESC_CODE_HIGH4  ; 第1个代码段描述符
    
    DATA_DESC:  dd 0x0000ffff
                dd DESC_DATA_HIGH4  ; 第2个数据段描述符

    VIDEO_DESC: dd 0x80000007
                dd DESC_VIDEO_HIGH4 ; 第3个显卡段描述符

    GDT_SIZE    equ $-GDT_BASE    ; GDT描述符表大小
    GDT_LIMIT   equ GDT_SIZE-1    ; GDT描述符表界限

    times RESERVE_GDT_NUM dq 0      ; 预留的GDT描述符

    SELECTOR_CODE equ (0x0001 << 3)+TI_GDT+RPL_0  ;代码段选择子
    SELECTOR_DATA equ (0x0002 << 3)+TI_GDT+RPL_0  ;数据段选择子
    SELECTOR_VIDEO equ (0x0003 << 3)+TI_GDT+RPL_0 ;显卡段选择子

    PTR_GDT   dw GDT_LIMIT
              dd GDT_BASE   ; GDTR指针变量

    setup_string_msg db 'ready in protect mode...' ; msg信息打印

; 准备进入保护模式
setup_start:
    mov sp,STACK_TOP_ADDR
    ; 获取内存分布（实模式）：后续进入保护模式内核需要(三种方法都使用)

    ; 显示msg信息：表示准备进入保护模式
    mov bp,setup_string_msg
    mov cx,24
    mov ax,0x1301
    mov bx,0x001f
    mov dx,0x1800
    int 0x10

    ;准备进入保护模式
    ;1. 打开A20
    in al,0x92
    or al,0000_0010b
    out 0x92,al
    ;2. 加载GDT
    lgdt [PTR_GDT] 
    ;3. 使能CR0打开保护模式标志位
    mov eax,cr0
    or eax,0x00000001
    mov cr0,eax
    ; 刷新流水线
    jmp SELECTOR_CODE:protect_mode_start

[bits 32]
protect_mode_start:
    ; 初始化段寄存器
    mov ax,SELECTOR_DATA
    mov ds,ax
    mov es,ax
    mov gs,ax
    mov ss,ax
    mov esp,STACK_TOP_ADDR ; 栈顶指针指向0x900
    mov ax,SELECTOR_VIDEO
    mov fs,ax
    ; msg显示
    mov byte [fs:160] ,'E'
    mov byte [fs:162] ,'n'
    mov byte [fs:164] ,'t'
    mov byte [fs:166] ,'e'
    mov byte [fs:168] ,'r'
    mov byte [fs:170] ,' '    
    mov byte [fs:172] ,'P'
    mov byte [fs:174] ,'_'
    mov byte [fs:176] ,'M'
    mov byte [fs:178] ,'o'
    mov byte [fs:180] ,'d'
    mov byte [fs:182] ,'e'
    mov byte [fs:184] ,'!'
    jmp $
