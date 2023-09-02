nasm -o boot.bin boot/bootload.s -I Include/boot/macro.inc
nasm -o setup.bin boot/setup.s -I Include/boot/macro.inc
gcc-4.4 -c -o  main.o kernel/main.c -m32
ld main.o -Ttext 0xc0001500 -e main -o kernel.bin -m elf_i386
dd if=boot.bin of=/home/pengsun/linux/tool/bochs/hd60M.img bs=512 count=1 conv=notrunc 
dd if=setup.bin of=/home/pengsun/linux/tool/bochs/hd60M.img bs=512 count=4 conv=notrunc seek=2 
dd if=kernel.bin of=/home/pengsun/linux/tool/bochs/hd60M.img bs=512 count=200 conv=notrunc seek=9 
rm boot.bin
rm setup.bin
rm kernel.bin
rm main.o

