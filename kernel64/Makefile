#####################################################
# Set Build Environment and Rules
#####################################################
NASM64 = nasm -f elf64
GCC64 = x86_64-pc-linux-gcc -c -m64 -ffreestanding
LD64 = x86_64-pc-linux-ld -melf_x86_64 -T ../elf_x86_64.x -nostdlib -e main -Ttext 0x200000
OBJCOPY64 = x86_64-pc-linux-objcopy -j .text -j .data -j .rodata -j .bss -S -O binary

OBJECTDIRECTORY = temp
SOURCEDIRECTORY = source

#####################################################
# Set Build Property and Method
#####################################################
all: prepare kernel64.bin

prepare:
	mkdir -p $(OBJECTDIRECTORY)

dep:
	@echo === Make Depenency File ===
	make -C $(OBJECTDIRECTORY) -f ../Makefile internalDependency
	@echo === Dependency Search Complete ===

executeInternalBuild: dep
	@echo === executeInternalBuild Start ===
	make -C $(OBJECTDIRECTORY) -f ../Makefile kernel64.elf	
	@echo === executeInternalBuild End ===

kernel64.bin: executeInternalBuild
	@echo === kernel64.bin Start ===
	$(OBJCOPY64) $(OBJECTDIRECTORY)/kernel64.elf $@
	@echo === kernel64.bin End ===

clean:
	rm -f *.bin
	rm -f $(OBJECTDIRECTORY)/*.*

#####################################################
# The part called from other make
# This part is executed along the Temp directory
#####################################################
# Define entry point file
ENTRYPOINTSOURCEFILE = ../$(SOURCEDIRECTORY)/entryPoint.s
ENTRYPOINTOBJECTFILE = entryPoint.o
# Define C file
CSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.c)
ASSEMBLYSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.asm)
COBJECTFILES = $(notdir $(patsubst %.c, %.o, $(CSOURCEFILES)))
ASSEMBLYOBJECTFILES = $(notdir $(patsubst %.asm,%.o,$(ASSEMBLYSOURCEFILES)))

# Build assembly entry point
$(ENTRYPOINTOBJECTFILE): $(ENTRYPOINTSOURCEFILE)
	$(NASM64) -g -o $@ $<

# Change .c to .o
%.o: ../$(SOURCEDIRECTORY)/%.c
	@echo === *.c to *.o  Start ===
	$(GCC64) -g -c $<
	@echo === *.c to *.o End ===

# Change .asm to .o
%.o: ../$(SOURCEDIRECTORY)/%.asm
	@echo === *.asm to *.o Start ===
	$(NASM64) -g -o $@ $<
	@echo === *.asm to *.o End ===

internalDependency:
	$(GCC64) -MM $(CSOURCEFILES) > dependency.dep

kernel64.elf: $(ENTRYPOINTOBJECTFILE) $(COBJECTFILES) $(ASSEMBLYOBJECTFILES)
	@echo === kernel64.elf Start ===
	$(LD64) -o $@ $^
	@echo === kernel64.elf End ===

ifeq (dependency.dep, $(wildcard dependency.dep))
include dependency.dep
endif



