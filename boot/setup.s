; 包含宏定义头文件
%include "Include/boot/macro.inc"
; 告诉汇编器将下面的程序编址起始为0x900
SECTION SETUP vstart=SETUP_BASE_ADDR
STACK_TOP_ADDR equ SETUP_BASE_ADDR

    ; 跳转至setup入口进行启动保护模式的准备
    jmp setup_start

    ;----------------------------------------------- 数据区域 ----------------------------------------------------

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
              dd GDT_BASE                         ; GDTR指针变量

    Physical_memory_capacity dd 0                 ; 物理内存容量访问地址0xb09
    Ards_buff dd 0                                ; Ards结构体缓存
              dd 0
              dd 0
              dd 0
              dd 0
    Ards_cnt  dw 0                                ; Ards结构体返回个数

    setup_string_msg db 'get memory capacity ok and ready in protect mode...' ; 读取内存ok并准备进入保护模式msg信息打印
    MSG_SETUP_LEN equ $ - setup_string_msg
    get_mem_capacity_error_string_msg db 'get memory capacity failed!'        ; 读取内存失败并挂起计算机msg信息打印
    MSG_GET_MEM_CAPACITY_ERROR_LEN equ $ - get_mem_capacity_error_string_msg

    ;----------------------------------------------- 代码区域 ----------------------------------------------------

; 获取内存尺寸并准备进入保护模式
setup_start:
    mov sp,STACK_TOP_ADDR
    ; 获取内存分布（实模式）：后续进入保护模式内核需要(三种方法都使用)

    ; 1.使用bios的中断0x15子功能0xE820获取物理内存
    sub ebx,ebx
    mov di,Ards_buff
    mov edx,SIGNATURE_MARKUP
one_get_phys_mem:
    mov eax,0xe820                  
    mov ecx,ARDS_SIZE
    int 0x15
    jc  two_get_phys_mem                ; 调用出错跳转到第二种获取物理内存的方法
    mov eax,[di]                        ; 保存获取到的最新内存容量
    add eax,[di + 8]   
    mov ecx,[Physical_memory_capacity]  ; 保存当前最大物理内存到ecx
    cmp eax,ecx                         ; 比较最新值与前一次的值
    jg  reg_to_mem                      ; 最新值更大则更新物理内存容量值，否则判断是否ards结构体全部获取,以此选择结束或者进入下一次获取
    inc word [Ards_cnt]
    jmp is_over
reg_to_mem:              
    mov [Physical_memory_capacity],eax  ; 如果最新值大于最大值则保存到Physical_memory_capacity中
    inc word [Ards_cnt]
is_over:                                ; 判断是否为最后一个结构体
    cmp ebx,0
    jnz one_get_phys_mem
    jmp get_mem_ok

    ; 2.使用bios的中断0x15子功能0xE801获取物理内存
two_get_phys_mem:
    mov eax,0xe801
    int 0x15
    jc  three_get_phys_mem              ; 失败则调用三号方法获取物理内存
    mov cx,0x400
    mul cx                              ; 这里转成字节数的方法是因为mul的原因
    shl edx,16
    and eax,0x0000ffff
    or  edx,eax
    add edx,0x100000
    mov esi,edx         

    sub eax,eax
    mov ax,bx
    mov ecx,0x10000
    mul ecx
    
    add esi,eax
    mov [Physical_memory_capacity],esi  ; 得到的内存容量放置到Physical_memory_capacity
    jmp get_mem_ok

    ; 3.使用bios的中断0x15子功能0x88获取物理内存:只能获取小于64M的内存
three_get_phys_mem:                 
    mov ah,0x88
    int 0x15
    jc  get_mem_error
    mov cx,0x400
    mul cx
    shl edx,16
    and eax,0x0000ffff
    or  edx,eax
    add edx,0x100000 
    mov [Physical_memory_capacity],edx    
    jmp get_mem_ok
    
    ; 三种方法都无法获取内存容量，计算机挂起
get_mem_error:                          
    ; 显示读取内存失败msg信息：并且挂起计算机
    mov bp,get_mem_capacity_error_string_msg
    mov cx,MSG_GET_MEM_CAPACITY_ERROR_LEN
    mov ax,0x1301
    mov bx,0x001f
    mov dx,0x1700
    int 0x10 
    jmp $   

get_mem_ok:                             ; 读取物理内存OK
    ; 显示msg信息：表示准备进入保护模式
    mov bp,setup_string_msg
    mov cx,MSG_SETUP_LEN
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
