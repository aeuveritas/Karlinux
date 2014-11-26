all: bootLoader.bin

bootLoader.bin: bootLoader.asm
	nasm -o bootLoader.bin bootLoader.asm
	
clean:
	rm -f bootLoader.bin