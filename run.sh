nasm -o boot.bin boot/bootload.s -I Include/boot/macro.inc
nasm -o setup.bin boot/setup.s -I Include/boot/macro.inc
dd if=boot.bin of=/home/pengsun/linux/tool/bochs/hd60M.img bs=512 count=1 conv=notrunc 
dd if=setup.bin of=/home/pengsun/linux/tool/bochs/hd60M.img bs=512 count=4 conv=notrunc seek=2 
rm boot.bin
rm setup.bin



