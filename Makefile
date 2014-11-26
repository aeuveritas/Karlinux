all: BootLoader Disk.img

BootLoader:
	@echo
	@echo ======= Build Boot Loader =======
	@echo
		
	make -C bootLoader
	
	@echo 
	@echo ======= Build Complete =======
	@echo 
	
Disk.img: bootLoader/bootLoader.bin
	@echo
	@echo ======= Build Boot Loader =======
	@echo
		
	cp bootLoader/bootLoader.bin Disk.img
	
	@echo 
	@echo ======= All Build Complete =======
	@echo 
	
clean:
	make -C bootLoader clean
	rm -f Disk.img
	