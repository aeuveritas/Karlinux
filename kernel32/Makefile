all: kernel32.bin

kernel32.bin: source/entryPoint.s
	nasm $< -f elf -g -o kernel32.elf
	x86_64-pc-linux-objcopy -O binary kernel32.elf kernel32.bin

clean:
	rm -f kernel32.bin
	rm -f kernel32.elf

