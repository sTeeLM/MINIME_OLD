CFLAGS=-O2 -Wall

all: mbr fixmbr

mbr: mbr512
	dd if=$< of=$@ bs=440 count=1

mbr512: mbr.S 
	gcc -m32 -c mbr.S -I. -DNO_BANNER -DNO_CHS -o mbr.o
	ld -melf_i386 -e start -Ttext 0x600 mbr.o -o mbr_tmp
	objcopy -O binary mbr_tmp mbr512

clean:
	-@rm -f mbr.o mbr mbr_tmp mbr512 fixmbr *~
