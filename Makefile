all: BootLoader Kernel32 Kernel64 Disk.img

BootLoader:
	@echo
	@echo ======= Build Boot Loader =======
	@echo
		
	make -C bootLoader
	
	@echo 
	@echo ======= Build Boot Loader Complete =======
	@echo 
	
Kernel32:
	@echo
	@echo ======= Build 32bit Kernel =======
	@echo
		
	make -C kernel32
	
	@echo 
	@echo ======= Build 32bit Complete =======
	@echo 
	
Kernel64:
	@echo
	@echo ======= Build 64bit Kernel =======
	@echo
		
	make -C kernel64
	
	@echo 
	@echo ======= Build 64bit Complete =======
	@echo 

Disk.img: bootLoader/bootLoader.bin kernel32/kernel32.bin kernel64/kernel64.bin
	@echo
	@echo ======= DISK Image Build Start =======
	@echo

	./imageMaker $^	
	
	@echo 
	@echo ======= All Build Complete =======
	@echo 
	
clean:
	make -C bootLoader clean
	make -C kernel32 clean
	make -C kernel64 clean
	rm -f disk.img
