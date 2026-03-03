CROSS ?= aarch64-linux-gnu-

CC      := $(CROSS)gcc
OBJCOPY := $(CROSS)objcopy

CPPFLAGS ?= -I./include
CFLAGS   ?= -ffreestanding -nostdlib -nostartfiles -Os -g

BL1_ELF := bl1.elf
BL1_BIN := bl1.bin
OBJS    := start.o main.o bl1_info.o bootrom_funcs.o bootdev.o epbl_info.o epbl_checks.o epbl_loader.o pmu.o string.o

all: $(BL1_BIN)

%.o: %.S
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BL1_ELF): $(OBJS) linker.ld
	$(CC) $(CPPFLAGS) $(CFLAGS) -T linker.ld $(OBJS) -o $@

$(BL1_BIN): $(BL1_ELF) bl1-footer patch_bl1_header.py
	$(OBJCOPY) -O binary $< $@
	truncate -s 9920 $@
	cat bl1-footer >> $@
	python3 patch_bl1_header.py $@

.PHONY: all clean

clean:
	rm -f $(BL1_BIN) $(BL1_ELF) $(OBJS)
