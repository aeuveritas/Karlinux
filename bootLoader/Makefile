all: bootLoader.bin

bootLoader.bin: bootLoader.asm
	nasm bootLoader.asm -f elf -g -o bootLoader.elf
	x86_64-pc-linux-objcopy -O binary bootLoader.elf bootLoader.bin
	
clean:
	rm -f bootLoader.bin
	rm -f bootLoader.elf

