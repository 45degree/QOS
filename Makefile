all: bootstrap kernel writeBytes
	dd if=/dev/zero of=system.img bs=1 count=1474560
	./writeBytes -i bootstrap -o system.img -s 0
	./writeBytes -i kernel -o system.img -s 18944
	rm bootstrap writeBytes kernel

bootstrap: bootstrap.asm
	nasm bootstrap.asm -o bootstrap

kernel: kernel.asm
	nasm kernel.asm -o kernel

writeBytes: tools/writeBytes.c
	gcc tools/writeBytes.c -o writeBytes

clear:
	rm *.img
