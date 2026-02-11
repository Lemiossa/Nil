# Makefile

ARCH := i386

LIBDIR := $(CURDIR)/Lib
BUILDDIR := $(CURDIR)/Build
BINDIR := $(BUILDDIR)/Bin
OBJDIR := $(BUILDDIR)/Obj
DEPDIR := $(BUILDDIR)/Dep

BOOTLOADER := $(BINDIR)/Bootldr.bin
FLOPPY := ./Nil_$(ARCH).flp

NASM := nasm
CC := bcc
LD := ld86

export LIBDIR
export BUILDDIR
export BINDIR
export OBJDIR
export DEPDIR
export NASM
export CC
export LD
export OBJCOPY

.PHONY: all
all: floppy

.PHONY: clean
clean:
	$(MAKE) -C Bootldr clean
	rm -rf $(FLOPPY)

.PHONY: qemu
qemu:  floppy
	qemu-system-$(ARCH) -drive file=$(FLOPPY),format=raw,if=floppy,media=disk \
		-audiodev pa,id=audio0 \
		-machine pc,pcspk-audiodev=audio0 \
		-serial stdio

.PHONY: bootloader
bootloader:
	$(MAKE) -C Bootldr TARGET=$(BOOTLOADER)

.PHONY: floppy
floppy: $(FLOPPY)

$(FLOPPY): bootloader
	truncate -s 1440K $@
	/sbin/mkfs.fat -F 12 -n "NIL" -R 64 $@
	dd if=$(BOOTLOADER) of=$@ bs=1 count=3 conv=notrunc
	dd if=$(BOOTLOADER) of=$@ bs=1 seek=62 skip=62 count=448 conv=notrunc
	dd if=$(BOOTLOADER) of=$@ bs=1 seek=512 skip=512 conv=notrunc
	printf "Hello World\r\n" > text.txt
	mcopy -i $(FLOPPY) text.txt "::/"
