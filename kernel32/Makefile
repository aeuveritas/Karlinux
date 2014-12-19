#####################################################
# Set Build Environment and Rules
#####################################################
NASM32 = nasm
GCC32 = x86_64-pc-linux-gcc -c -m32 -ffreestanding
LD32 = x86_64-pc-linux-ld -melf_i386 -T ../elf_i386.x -nostdlib -e main -Ttext 0x10200
OBJCOPY32 = x86_64-pc-linux-objcopy -j .text -j .data -j .rodata -j .bss -S -O binary

OBJECTDIRECTORY = temp
SOURCEDIRECTORY = source

#####################################################
# Set Build Property and Method
#####################################################
all: prepare kernel32.bin

prepare:
	mkdir -p $(OBJECTDIRECTORY)

$(OBJECTDIRECTORY)/entryPoint.bin: $(SOURCEDIRECTORY)/entryPoint.s
	@echo === entryPoint Start ===
	$(NASM32) -g -o $@ $<
	@echo === entryPoint End ===

dep:
	@echo === Make Depenency File ===
	make -C $(OBJECTDIRECTORY) -f ../Makefile internalDependency
	@echo === Dependency Search Complete ===

executeInternalBuild: dep
	@echo === executeInternalBuild Start ===
	make -C $(OBJECTDIRECTORY) -f ../Makefile kernel32.elf	
	@echo === executeInternalBuild End ===

$(OBJECTDIRECTORY)/kernel32.elf.bin: executeInternalBuild
	@echo === kernel32.elf.bin Start ===
	$(OBJCOPY32) $(OBJECTDIRECTORY)/kernel32.elf $@
	@echo === kernel32.elf.bin End ===

kernel32.bin: $(OBJECTDIRECTORY)/entryPoint.bin $(OBJECTDIRECTORY)/kernel32.elf.bin
	@echo === kernel32.bin Start ===
	cat $^ > $@
	@echo === kernel32.bin End ===

clean:
	rm -f *.bin
	rm -f $(OBJECTDIRECTORY)/*.*

#####################################################
# The part called from other make
# This part is executed along the Temp directory
#####################################################
CENTRYPOINTOBJECTFILE = main.o
CSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.c)
ASSEMBLYSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.asm)
COBJECTFILES = $(subst main.o, , $(notdir $(patsubst %.c,%.o,$(CSOURCEFILES))))
ASSEMBLYOBJECTFILES = $(notdir $(patsubst %.asm,%.o,$(ASSEMBLYSOURCEFILES)))

# Change .c to .o
%.o: ../$(SOURCEDIRECTORY)/%.c
	@echo === *.c to *.o  Start ===
	$(GCC32) -g -c $<
	@echo === *.c to *.o End ===

# Change .asm to .o
%.o: ../$(SOURCEDIRECTORY)/%.asm
	@echo === *.asm to *.o Start ===
	$(NASM32) -f elf32 -g -o $@ $<
	@echo === *.asm to *.o End ===

internalDependency:
	$(GCC32) -MM $(CSOURCEFILES) > dependency.dep

kernel32.elf: $(CENTRYPOINTOBJECTFILE) $(COBJECTFILES) $(ASSEMBLYOBJECTFILES)
	@echo === kernel32.elf Start ===
	$(LD32) -o $@ $^
	@echo === kernel32.elf End ===

ifeq (dependency.dep, $(wildcard dependency.dep))
include dependency.dep
endif



