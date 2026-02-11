# Makefile

NASM ?= nasm

SOURCEDIR := $(CURDIR)

LIBDIR ?= ../Lib
BUILDDIR ?= $(CURDIR)/Build
BINDIR ?= $(BUILDDIR)/Bin
OBJDIR ?= $(BUILDDIR)/Obj
DEPDIR ?= $(BUILDDIR)/Dep

TARGET ?= $(BINDIR)/Bootldr.bin

ifeq ($(BUILDDIR),$(CURDIR))
	BUILDDIR := $(CURDIR)/Build
endif

NASM ?= nasm
CC ?= bcc
LD ?= ld86

BOOTLOADER_LOC := 0x8000

NASMFLAGS := -f as86 -O0
CFLAGS := -0 -O -ansi
LDFLAGS := -d -s -T $(BOOTLOADER_LOC)

SOURCE := \
		  Entry.asm \
		  Bios.c \
		  Util.c \
		  Main.c
SOURCE := $(addprefix $(SOURCEDIR)/,$(SOURCE))

SOURCE_ASM := $(filter %.asm,$(SOURCE))
SOURCE_C := $(filter %.c,$(SOURCE))

OBJ_C := $(patsubst $(SOURCEDIR)/%.c,$(OBJDIR)/Boot/C/%.o,$(SOURCE_C))
OBJ_ASM := $(patsubst $(SOURCEDIR)/%.asm,$(OBJDIR)/Boot/Asm/%.o,$(SOURCE_ASM))

OBJ := $(OBJ_ASM) $(OBJ_C)

DEP_C := $(patsubst $(SOURCEDIR)/%.c,$(DEPDIR)/Boot/C/%.d,$(SOURCE_C))
DEP_ASM := $(patsubst $(SOURCEDIR)/%.asm,$(DEPDIR)/Boot/Asm/%.d,$(SOURCE_ASM))

DEP := $(DEP_ASM) $(DEP_C)

LIBS := $(LIBDIR)/Bcc/bccmath.a

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

$(BINDIR)/Boot/Bootsect.bin: $(SOURCEDIR)/Bootsect.asm
	mkdir -p $(dir $@)
	$(NASM) -f bin $< -o $@ -DADDR=$(BOOTLOADER_LOC)

$(BINDIR)/Boot/Loader.bin: $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

$(TARGET): $(BINDIR)/Boot/Bootsect.bin $(BINDIR)/Boot/Loader.bin
	mkdir -p $(dir $@)
	cat $^ > $@

$(OBJDIR)/Boot/Asm/%.o: $(SOURCEDIR)/%.asm
	mkdir -p $(dir $@) $(dir $(DEPDIR)/$*)
	$(NASM) $(NASMFLAGS) $< -o $@ -MD -MF $(DEPDIR)/$*.d

$(OBJDIR)/Boot/C/%.o: $(SOURCEDIR)/%.c
	mkdir -p $(dir $@) $(dir $(DEPDIR)/$*)
	$(CC) $(CFLAGS) -o $@ -c $<

-include $(DEP)
