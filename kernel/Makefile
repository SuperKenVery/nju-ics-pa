LIB_COMMON_DIR = ../libs
NEWLIBC_DIR = $(LIB_COMMON_DIR)/newlib

CC = gcc
CFLAGS = -O0 -m32 -MMD -Wall -Werror -Wno-cpp -march=i386 -fno-builtin -fno-omit-frame-pointer -fno-stack-protector -I./include -I../include -I$(NEWLIBC_DIR)/include
ASFLAGS = -m32 -MMD -I./include -I../include
# LDFLAGS = -Ttext=0x30000 -m elf_i386 # before page
LDFLAGS = -O0 -Ttext=0xc0030000 -m elf_i386 

CSRC = $(shell find ./src -name "*.c")
SSRC = $(shell find ./src -name "*.S")

OBJS = $(SSRC:.S=.o) $(CSRC:.c=.o)
POST_PREPROCESS = $(CSRC:.c=.post_pre)
START_OBJ = start/start.o
MM_MALLOC_OBJ = src/memory/mm_malloc.o 
ALL_OBJS = $(OBJS) $(START_OBJ) 

NEWLIBC = $(NEWLIBC_DIR)/libc.a

kernel: $(ALL_OBJS) ../include/config.h Makefile
	ld $(LDFLAGS) -e start -o kernel $(START_OBJ) $(OBJS) $(MM_MALLOC_OBJ) $(NEWLIBC)
	objcopy --remove-section .note.gnu.property kernel
	objcopy -S -O binary kernel kernel.img

# Thanks to ChatGPT!
%.post_pre: %.c
	$(CC) $(CFLAGS) -E $< -o $@

post: $(POST_PREPROCESS)

-include $(ALL_OBJS:.o=.d)

clean:
	-rm -f kernel kernel.img $(ALL_OBJS) $(ALL_OBJS:.o=.d)
