#####################################################
# Set Build Environment and Rules
#####################################################
NASM32 = nasm
GCC32 = x86_64-pc-linux-gcc -c -m32 -ffreestanding
LD32 = x86_64-pc-linux-ld -melf_i386 -T ./elf_i386 -nostdlib -e Main -Ttext 0x10200
OBJCOPY32 = x86_64-pc-linux-objcopy -j .text -k .data -j .rodata -j .bss -S -O binary

OBJECTDIRECTORY = temp
SOURCEDIRECTORY = source

#####################################################
# Set Build Property and Method
#####################################################
all: prepare kernel32.bin

prepare:
	mkdir -p $(OBJECTDIRECTORY)

$(OBJECTDIRECTORY)/entryPoint.bin: $(SOURCEDIRECTORY)/entryPoint.s
	$(NASM32) -g -o $@ $<

dep:
	@echo === Make Depenency File ===
	make -C $(OBJECTDIRECTORY) -f ../Makefile internalDependency
	@echo === Dependency Search Complete ===

executeInternalBuild: dep
	make -C $(OBJECTDIRECTORY) -f ../Makefile kernel32.elf	

$(OBJECTDIRECTORY)/kernel32.elf.bin: executeInternalBuild
	$(OBJCOPY32) $(OBJECTDIRECTORY)/kernel32.elf $@

kernel32.bin: $(OBJECTDIRECTORY)/entryPoint.bin $(OBJECTDIRECTORY)/kernel32.elf.bin
	cat $^ > $@

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
ASSEMBLYOBJECTFILES = $(notdir $(patsubst %.asm,%.o,$(ASSEMBKYSOURCEFILES)))

# Change .c to .o
%.o: ../$(SOURCEFIRECTORY)/%.c
	$(GCC32) -g -c -o $@ $<

# Change .asm to .o
%.o: ../$(SOURCEDIRECTORY)/%.asm
	$(NASM32) -f elf32 -g -o $@ $<

internalDependency:
	$(GCC32) -MM $(CSOURCEFILES) > dependency.dep

kernel32.elf: $(CENTRYPOINTOBJECTFILE) $(CSOURCEFILES) $(ASSEMBLYOBJECTFILES)
	$(LD32) -o $@ $^

ifeq (dependency.dep, $(wildcard depedency.dep))
include dependency.dep
endif



