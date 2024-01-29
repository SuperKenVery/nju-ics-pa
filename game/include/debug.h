#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "common.h"
#include "x86.h"

#define GAME_LABEL " {game} "

#define Log(format, ...)                                       \
	printf("\33[1;35m[%s,%d,%s]" GAME_LABEL format "\33[0m\n", \
		   __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define panic(format, ...)                                    \
	do                                                        \
	{                                                         \
		cli();                                                \
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

#endif
