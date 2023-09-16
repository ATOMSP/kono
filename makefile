# 路径宏定义
BUILE_DIR = ./build
C_ENTER_ADDR = 0xc0001500
HD60M_PATH = ../../tool/bochs-gdb/hd60M.img
# 构建工具宏定义
ASM = nasm
CC  = gcc-4.4
LD  = ld
# 构建参数宏定义
ASLIB = -I Include/boot
CLIB = -I Include/
ASFLAGS = -f elf -g# 汇编文件编译成elf格式
CFLAGS = -Wall $(CLIB) -c -fno-builtin -W -Wstrict-prototypes -Wmissing-prototypes -m32 -g
LDFLAGS = -Ttext $(C_ENTER_ADDR) -e main -Map $(BUILE_DIR)/kernel.map -m elf_i386
ISDEBUG = -D NO_DEBUG # 是否打开debug

# 内核目标文件: 添加c源文件时，需要修改
OBJS = $(BUILE_DIR)/main.o $(BUILE_DIR)/init.o $(BUILE_DIR)/thread.o $(BUILE_DIR)/mm.o  $(BUILE_DIR)/bitmap.o $(BUILE_DIR)/string.o \
       $(BUILE_DIR)/debug.o $(BUILE_DIR)/interrupt.o  $(BUILE_DIR)/s_interrupt.o  $(BUILE_DIR)/print.o 

# 编译引导程序和启动代码
boot:$(BUILE_DIR)/bootload.o $(BUILE_DIR)/setup.o
$(BUILE_DIR)/bootload.o:boot/bootload.s	 
	$(ASM) $(ASLIB) -o $(BUILE_DIR)/bootload.o boot/bootload.s	 
$(BUILE_DIR)/setup.o:boot/setup.s	 
	$(ASM) $(ASLIB) -o $(BUILE_DIR)/setup.o boot/setup.s	 

# 编译内核C语言代码: 添加c源文件时，需要修改
$(BUILE_DIR)/main.o:kernel/main.c
	$(CC) $(CFLAGS) $(CLIB) -o $@ $<
$(BUILE_DIR)/init.o:kernel/init.c
	$(CC) $(CFLAGS) $(CLIB) -o $@ $<
$(BUILE_DIR)/thread.o:lib/kernel/thread.c
	$(CC) $(CFLAGS) $(CLIB) -o $@ $<
$(BUILE_DIR)/mm.o:kernel/mm.c
	$(CC) $(CFLAGS) $(CLIB) -o $@ $<
$(BUILE_DIR)/bitmap.o:lib/kernel/bitmap.c
	$(CC) $(CFLAGS) $(CLIB) -o $@ $<
$(BUILE_DIR)/interrupt.o:kernel/interrupt.c
	$(CC) $(CFLAGS) $(CLIB) -o $@ $<
$(BUILE_DIR)/debug.o:kernel/debug.c
	$(CC) $(CFLAGS) $(CLIB) -o $@ $<
$(BUILE_DIR)/string.o:lib/kernel/string.c
	$(CC) $(CFLAGS) $(CLIB) -o $@ $<
# 编译内核汇编代码
$(BUILE_DIR)/print.o:lib/kernel/print.s
	$(ASM) $(ASLIB) $(ASFLAGS) -o $@ $<
$(BUILE_DIR)/s_interrupt.o:kernel/interrupt.s
	$(ASM) $(ASLIB) $(ASFLAGS) -o $@ $< 
    
# 链接所有内核目标文件
$(BUILE_DIR)/kernel.bin:$(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^


.PHONY: mk_dir hd clean build all boot gdb_symbol

gdb_symbol:
	objcopy --only-keep-debug $(BUILE_DIR)/kernel.bin $(BUILE_DIR)/kernel.sym

mk_dir: 
	if [ ! -d $(BUILE_DIR) ];then mkdir $(BUILE_DIR);fi

hd:
	dd if=$(BUILE_DIR)/bootload.o of=$(HD60M_PATH) bs=512 count=1 conv=notrunc && \
	dd if=$(BUILE_DIR)/setup.o of=$(HD60M_PATH) bs=512 count=4 conv=notrunc seek=2 && \
	dd if=$(BUILE_DIR)/kernel.bin of=$(HD60M_PATH) bs=512 count=200 conv=notrunc seek=9 	

clean:
	@cd $(BUILE_DIR) && rm -f ./* && echo "remove build files ok!"

build: $(BUILE_DIR)/kernel.bin
# 编译后直接进入gdb调试
run: 
	gdb 
# 编译并下载镜像
all: mk_dir boot build hd gdb_symbol run




















