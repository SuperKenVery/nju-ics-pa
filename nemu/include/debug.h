#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>

// Copied from https://gist.github.com/ccbrown/9722406
// void _do_hexdump(const void* data, size_t size) {
#define hexdump(var)                                                            \
{                                                                               \
	char ascii[17];                                                               \
	size_t i, j;                                                                  \
	ascii[16] = '\0';                                                             \
	for (i = 0; i < size; ++i) {                                                  \
		printf("%02X ", ((unsigned char*)data)[i]);                                 \
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') { \
			ascii[i % 16] = ((unsigned char*)data)[i];                                \
		} else {                                                                    \
			ascii[i % 16] = '.';                                                      \
		}                                                                           \
		if ((i+1) % 8 == 0 || i+1 == size) {                                        \
			printf(" ");                                                              \
			if ((i+1) % 16 == 0) {                                                    \
				printf("|  %s \n", ascii);                                              \
			} else if (i+1 == size) {                                                 \
				ascii[(i+1) % 16] = '\0';                                               \
				if ((i+1) % 16 <= 8) {                                                  \
					printf(" ");                                                          \
				}                                                                       \
				for (j = (i+1) % 16; j < 16; ++j) {                                     \
					printf("   ");                                                        \
				}                                                                       \
				printf("|  %s \n", ascii);                                              \
			}                                                                         \
		}                                                                           \
	}                                                                             \
}                                                                               \

// https://stackoverflow.com/questions/7766115/print-variable-name-in-a-define
// #define hexdump(var)               \
//   {                                \
//     printf("%s:\n",#var);          \
//     _do_hexdump(&var,sizeof(var)); \
//   }

/* A convenient function named breakpoint
 * 
 * You can insert this function anywhere, probably between `if`s, 
 * and just `break dbgbreak` in gdb. 
*/
void dbgbreak(){
  printf("dbgbreak: Don't optimize me out!\n");
}



#endif
