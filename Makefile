all: BootLoader Kernel32 Disk.img

BootLoader:
	@echo
	@echo ======= Build Boot Loader =======
	@echo
		
	make -C bootLoader
	
	@echo 
	@echo ======= Build Complete =======
	@echo 
	
Kernel32:
	@echo
	@echo ======= Build 32bit Kernel =======
	@echo
		
	make -C kernel32
	
	@echo 
	@echo ======= Build Complete =======
	@echo 

Disk.img: BootLoader Kernel32
	@echo
	@echo ======= DISK Image Build Start =======
	@echo

	cat bootLoader/bootLoader.bin kernel32/virtualOS.bin > Disk.img		
	
	@echo 
	@echo ======= All Build Complete =======
	@echo 
	
clean:
	make -C bootLoader clean
	make -C kernel32 clean
	rm -f Disk.img
	
