[bits 32]

SECTION .text
;任务切换
global switch_to

switch_to:
  ; 保护当前执行流上下文  
  push esi
  push edi
  push ebx
  push ebp
  mov eax,[esp + 20]
  mov [eax],esp 
  ; 恢复下一个线程环境
  mov eax,[esp + 24]
  mov esp, [eax]
  ;esp指针指向新线程的栈，此时pop则会把新线程保存的环境pop回来，
  ;如果第一次上cpu的话，初始化的时候esp指向的是kernel_start，他的下面就是四个寄存器的坑位
  pop ebp
  pop ebx
  pop edi
  pop esi
  ret











