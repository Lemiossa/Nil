# Makefile

NASM ?= nasm

SOURCEDIR := $(CURDIR)
BUILDDIR ?= $(CURDIR)/Build
BINDIR ?= $(BUILDDIR)/Bin
OBJDIR ?= $(BUILDDIR)/Obj

TARGET ?= $(BINDIR)/Bootldr.bin

ifeq ($(BUILDDIR),$(CURDIR))
	BUILDDIR := $(CURDIR)/Build
endif

CC86 = bcc
LD86 = ld86

SOURCE := \
		  Entry.asm \
		  Main.c
SOURCE := $(addprefix $(SOURCEDIR)/,$(SOURCE))

SOURCE_ASM := $(filter %.asm,$(SOURCE))
SOURCE_C := $(filter %.c,$(SOURCE))

OBJ_C := $(patsubst $(SOURCEDIR)/%.c,$(OBJDIR)/Boot/C/%.o,$(SOURCE_C))
OBJ_ASM := $(patsubst $(SOURCEDIR)/%.asm,$(OBJDIR)/Boot/Asm/%.o,$(SOURCE_ASM))

OBJ := $(OBJ_ASM) $(OBJ_C)

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

$(BINDIR)/Boot/Bootsect.bin: $(SOURCEDIR)/Bootsect.asm
	mkdir -p $(dir $@)
	$(NASM) -f bin $< -o $@

$(BINDIR)/Boot/Loader.bin: $(OBJ)
	$(LD86) -d -s -T 0x500 -o $@ $^ /usr/lib/bcc/libc.a

$(TARGET): $(BINDIR)/Boot/Bootsect.bin $(BINDIR)/Boot/Loader.bin
	mkdir -p $(dir $@)
	cat $^ > $@

$(OBJDIR)/Boot/Asm/%.o: $(SOURCEDIR)/%.asm
	mkdir -p $(dir $@)
	$(NASM) -f as86 $< -o $@

$(OBJDIR)/Boot/C/%.o: $(SOURCEDIR)/%.c
	mkdir -p $(dir $@)
	$(CC86) -0 -O -W -ansi -o $@ -c $<
