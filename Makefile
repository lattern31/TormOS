CC=i686-elf-gcc
LD=i686-elf-ld
CFLAGS=-ffreestanding -nostdlib -g -c -Wall
QEMU=qemu-system-i386 
objects = obj/boot.o \
		  obj/vga.o \
		  obj/3d.o \
		  obj/terminal.o \
		  obj/gui.o \
		  obj/math.o \
		  obj/initX86.o \
		  obj/gdt.o \
		  obj/idt.o \
		  obj/isr.o \
		  obj/isr_asm.o \
		  obj/stdio.o \
		  obj/keyboard.o \
		  obj/pic.o \
		  obj/kernel.o \

obj/%.o: src/boot/%.s obj
	$(CC) $(CFLAGS) -o $@ $<

obj/%.o: src/%.s obj
	$(CC) $(CFLAGS) -o $@ $<

obj/%.o: src/%.c obj
	$(CC) $(CFLAGS) -o $@ $<

obj/%.o: src/vga/%.c obj
	$(CC) $(CFLAGS) -o $@ $<

obj: 
	mkdir -p $@

all: kernel.elf

kernel.elf: linker.ld $(objects)
	$(LD) -T $< -o $@ $(objects)

qemu: kernel.elf
	$(QEMU) -kernel kernel.elf

clean:
	rm -rf obj kernel.elf 

.PHONY: obj clean
