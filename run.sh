# nasm -o boot.bin boot/bootload.s -I Include/boot/macro.inc
# nasm -o setup.bin boot/setup.s -I Include/boot/macro.inc


gcc-4.4 -c -o  main.o kernel/main.c -m32 -I Include/kernel/ -I Include/lib
gcc-4.4 -c -o  interrupt.o kernel/interrupt.c -m32 -I Include/kernel/ -I Include/lib
gcc-4.4 -c -o  init.o kernel/init.c -m32 -I Include/kernel/ -I Include/lib
nasm -o s_interrupt.o -f elf kernel/interrupt.s 
nasm -o print.o -f elf lib/kernel/print.s -I Include/boot/macro.inc
ld main.o print.o init.o interrupt.o s_interrupt.o -Ttext 0xc0001500 -e main -o kernel.bin -m elf_i386 # 调用在前，实现在后


# dd if=boot.bin of=/home/pengsun/linux/tool/bochs/hd60M.img bs=512 count=1 conv=notrunc 
# dd if=setup.bin of=/home/pengsun/linux/tool/bochs/hd60M.img bs=512 count=4 conv=notrunc seek=2 
dd if=kernel.bin of=/home/pengsun/linux/tool/bochs/hd60M.img bs=512 count=200 conv=notrunc seek=9 
# rm boot.bin
# rm setup.bin
rm kernel.bin
rm main.o
rm print.o
rm interrupt.o
rm s_interrupt.o
rm init.o
