#ifndef __NEMU_DEBUG_H__
#define __NEMU_DEBUG_H__

#include <stdio.h>

// Copied from https://gist.github.com/ccbrown/9722406
// void _do_hexdump(const void* data, size_t size) {
#define hexdump(var)                                                                       \
{                                                                                          \
  printf("%s:\n",#var);                                                                    \
  const void* data=&(var);                                                                 \
  size_t size=sizeof(var);                                                                 \
	char ascii[17];                                                                          \
	size_t i, j;                                                                             \
	ascii[16] = '\0';                                                                        \
	for (i = 0; i < size; ++i) {                                                             \
		printf("%02X ", ((unsigned char*)data)[i]);                                            \
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {            \
			ascii[i % 16] = ((unsigned char*)data)[i];                                           \
		} else {                                                                               \
			ascii[i % 16] = '.';                                                                 \
		}                                                                                      \
		if ((i+1) % 8 == 0 || i+1 == size) {                                                   \
			printf(" ");                                                                         \
			if ((i+1) % 16 == 0) {                                                               \
				printf("|  %s \n", ascii);                                                         \
			} else if (i+1 == size) {                                                            \
				ascii[(i+1) % 16] = '\0';                                                          \
				if ((i+1) % 16 <= 8) {                                                             \
					printf(" ");                                                                     \
				}                                                                                  \
				for (j = (i+1) % 16; j < 16; ++j) {                                                \
					printf("   ");                                                                   \
				}                                                                                  \
				printf("|  %s \n", ascii);                                                         \
			}                                                                                    \
		}                                                                                      \
	}                                                                                        \
}                                                                                          \

#define hexdump_pointer(var,len)                                                                     \
{                                                                                                    \
  printf("%s:\n",#var);                                                                              \
  const void* data=(var);                                                                            \
  size_t size=len;                                                                                   \
	char ascii[17];                                                                                    \
	size_t i, j;                                                                                       \
	ascii[16] = '\0';                                                                                  \
	for (i = 0; i < size; ++i) {                                                                       \
		printf("%02X ", ((unsigned char*)data)[i]);                                                      \
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {                      \
			ascii[i % 16] = ((unsigned char*)data)[i];                                                     \
		} else {                                                                                         \
			ascii[i % 16] = '.';                                                                           \
		}                                                                                                \
		if ((i+1) % 8 == 0 || i+1 == size) {                                                             \
			printf(" ");                                                                                   \
			if ((i+1) % 16 == 0) {                                                                         \
				printf("|  %s \n", ascii);                                                                   \
			} else if (i+1 == size) {                                                                      \
				ascii[(i+1) % 16] = '\0';                                                                    \
				if ((i+1) % 16 <= 8) {                                                                       \
					printf(" ");                                                                               \
				}                                                                                            \
				for (j = (i+1) % 16; j < 16; ++j) {                                                          \
					printf("   ");                                                                             \
				}                                                                                            \
				printf("|  %s \n", ascii);                                                                   \
			}                                                                                              \
		}                                                                                                \
	}                                                                                                  \
}                                                                                                    \

#define greenprintf(...) \
{                        \
	printf("\e[0;32m");    \
	printf(__VA_ARGS__);   \
	printf("\e[0m");       \
}

/* A convenient function named breakpoint
 * 
 * You can insert this function anywhere, probably between `if`s, 
 * and just `break dbgbreak` in gdb. 
*/
void dbgbreak();



#endif
