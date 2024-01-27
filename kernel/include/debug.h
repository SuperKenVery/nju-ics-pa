#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "common.h"
#include "x86.h"
#include <stdarg.h>

#define KERNEL_LABEL " {kernel} "

void printk(const char *, ...);

#define Log(format, ...)                                         \
	printk("\33[1;35m[%s,%d,%s]" KERNEL_LABEL format "\33[0m\n", \
		   __FILE__, __LINE__, __func__, ##__VA_ARGS__);

#define panic(format, ...)                                    \
	do                                                        \
	{                                                         \
		Log("\33[1;31msystem panic: " format, ##__VA_ARGS__); \
		HIT_BAD_TRAP;                                         \
	} while (0)

#define assert(cond)                              \
	do                                            \
	{                                             \
		if (!(cond))                              \
		{                                         \
			panic("Assertion failed: %s", #cond); \
		}                                         \
	} while (0)


// Copied from https://gist.github.com/ccbrown/9722406
// void _do_hexdump(const void* data, size_t size) {
#define hexdump(var)                                                                                    \
{                                                                                                       \
  printk("%s:\n",#var);                                                                                 \
  const void* data=&(var);                                                                              \
  size_t size=sizeof(var);                                                                              \
	char ascii[17];                                                                                       \
	size_t i, j;                                                                                          \
	ascii[16] = '\0';                                                                                     \
	for (i = 0; i < size; ++i) {                                                                          \
		printk("%02X ", ((unsigned char*)data)[i]);                                                         \
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {                         \
			ascii[i % 16] = ((unsigned char*)data)[i];                                                        \
		} else {                                                                                            \
			ascii[i % 16] = '.';                                                                              \
		}                                                                                                   \
		if ((i+1) % 8 == 0 || i+1 == size) {                                                                \
			printk(" ");                                                                                      \
			if ((i+1) % 16 == 0) {                                                                            \
				printk("|  %s \n", ascii);                                                                      \
			} else if (i+1 == size) {                                                                         \
				ascii[(i+1) % 16] = '\0';                                                                       \
				if ((i+1) % 16 <= 8) {                                                                          \
					printk(" ");                                                                                  \
				}                                                                                               \
				for (j = (i+1) % 16; j < 16; ++j) {                                                             \
					printk("   ");                                                                                \
				}                                                                                               \
				printk("|  %s \n", ascii);                                                                      \
			}                                                                                                 \
		}                                                                                                   \
	}                                                                                                     \
}                                                                                                       \

#define hexdump_pointer(var,len)                                                                        \
{                                                                                                       \
  printk("%s:\n",#var);                                                                                 \
  const void* data=(var);                                                                               \
  size_t size=len;                                                                                      \
	char ascii[17];                                                                                       \
	size_t i, j;                                                                                          \
	ascii[16] = '\0';                                                                                     \
	for (i = 0; i < size; ++i) {                                                                          \
		printk("%02X ", ((unsigned char*)data)[i]);                                                         \
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {                         \
			ascii[i % 16] = ((unsigned char*)data)[i];                                                        \
		} else {                                                                                            \
			ascii[i % 16] = '.';                                                                              \
		}                                                                                                   \
		if ((i+1) % 8 == 0 || i+1 == size) {                                                                \
			printk(" ");                                                                                      \
			if ((i+1) % 16 == 0) {                                                                            \
				printk("|  %s \n", ascii);                                                                      \
			} else if (i+1 == size) {                                                                         \
				ascii[(i+1) % 16] = '\0';                                                                       \
				if ((i+1) % 16 <= 8) {                                                                          \
					printk(" ");                                                                                  \
				}                                                                                               \
				for (j = (i+1) % 16; j < 16; ++j) {                                                             \
					printk("   ");                                                                                \
				}                                                                                               \
				printk("|  %s \n", ascii);                                                                      \
			}                                                                                                 \
		}                                                                                                   \
	}                                                                                                     \
}                                                                                                       \


#endif
