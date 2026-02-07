# Makefile

NASM ?= nasm

SOURCEDIR := $(CURDIR)
BUILDDIR ?= $(CURDIR)/Build
BINDIR ?= $(BUILDDIR)/Bin

TARGET ?= $(BINDIR)/Bootldr.bin

ifeq ($(BUILDDIR),$(CURDIR))
	BUILDDIR := $(CURDIR)/Build
endif

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

$(BINDIR)/Boot/Bootsect.bin: $(SOURCEDIR)/Bootsect.asm
	mkdir -p $(dir $@)
	$(NASM) -f bin $< -o $@

$(TARGET): $(BINDIR)/Boot/Bootsect.bin
	mkdir -p $(dir $@)
	cat $^ > $@
