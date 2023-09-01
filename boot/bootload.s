; ------------------bootload.s文件-主引导扇区代码-负责将setup加载到内存0x900处------------------
; 包含宏定义头文件
%include "Include/boot/macro.inc"
; 告诉汇编器将下面的程序编址起始为0x7c00
SECTION MBR vstart=MBR_BASE_ADDR
    ; 清除段寄存器，设置栈指针指向0x7c00，因为栈向下生长，并且设置fs寄存器为0xb800，这样可以直接访问显卡 
    mov ax,cs
    mov ss,ax
    mov ds,ax
    mov es,ax
    mov gs,ax
    mov ax,0x7c00
    mov sp,ax
    mov ax,0xb800
    mov fs,ax


    ; 调用bios的中断，执行清屏操作
    mov ax,0x0600
    mov bx,0x0700
    mov cx,0x0000
    mov dx,0x184f
    int 0x10

    ; 直接写入显卡对应内存映射区域
    mov byte [fs:0x0000] ,'H'
    mov byte [fs:0x0001] ,0xA4
    
    mov byte [fs:0x0002] ,'e'
    mov byte [fs:0x0003] ,0xA4
    
    mov byte [fs:0x0004] ,'l'
    mov byte [fs:0x0005] ,0xA4
    
    mov byte [fs:0x0006] ,'l'
    mov byte [fs:0x0007] ,0xA4

    mov byte [fs:0x0008] ,'o'
    mov byte [fs:0x0009] ,0xA4

    mov byte [fs:0x000A] ,'!'
    mov byte [fs:0x000B] ,0xA4
        
	; 加载硬盘第2个扇区（这里使用的是LBA模式）
	mov eax,SETUP_START_SECTOR
	mov bx,SETUP_BASE_ADDR
	mov cx,SETUP_LOAD_SECTOR_NUM
	call read_disk_mode_in_16
	jmp SETUP_BASE_ADDR    
	;16位模式下读取硬盘
read_disk_mode_in_16:
	; 保存eax和cx的参数
	mov esi,eax
    mov di,cx
    ; 写硬盘控制器的0x1f2端口cl寄存器的值，表示要加载的扇区个数
	mov dx,0x01f2
	mov al,cl
	out dx,al
	; 下面开始写入lba模式下的扇区编号，这里是第2个扇区
	mov eax,esi
	mov dx,0x01f3
	out dx,al

	mov cl,8
	shr eax,cl
	mov dx,0x01f4
	out dx,al

    mov dx,0x01f5
	shr eax,cl
	out dx,al
	 
	shr eax,cl
	and al,0x0f
	or  al,0xe0
	mov dx,0x01f6
	out dx,al
	; 写入命令寄存器0x20，表示需要进行读硬盘操作
	mov dx,0x01f7
	mov al,0x20
	out dx,al
	; 等待硬盘准备好
.not_ready:
	nop
	in al,dx
	and al,0x88
	cmp al,0x08
	jnz .not_ready
	; 进行读取硬盘数据到指定位置0x900
	mov ax,di
	mov dx,256
	mul dx
	mov cx,ax
	mov dx,0x01f0
.reading_data:
	in ax,dx
	mov [bx],ax
	add bx,2
	loop .reading_data
	ret
    ; 填充其他空间数据
    times 510-($-$$) db 0
    db 0x55,0xaa

	


