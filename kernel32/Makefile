all: virtualOS.bin

virtualOS.bin: virtualOS.asm
	nasm virtualOS.asm -f elf -g -o virtualOS.elf
	x86_64-pc-linux-objcopy -O binary virtualOS.elf virtualOS.bin

clean:
	rm -f virtualOS.bin
	rm -f virtualOS.elf

