#include "common.h"
#include "debug.h"
#include <stdint.h>
#include <string.h>

#define WRITEBACK_TIME 1 /* writeback cache for every 1 second */
#define HZ 100

void cache_init(void);
void cache_writeback(void);
uint8_t read_byte(uint32_t);
void write_byte(uint32_t, uint8_t);

void add_irq_handle(int, void (*)(void));

#ifdef QUICK_DISK
const uint32_t QDISK_BASE=((uint32_t)3300)*1024*1024;
#endif

/* The loader acts as a monolithic kernel, therefore we do not need
 * to translate the address ``buf'' from the user process to a physical
 * one, which is necessary for a microkernel.
 */
void ide_read(uint8_t *buf, uint32_t offset, uint32_t len)
{
#ifndef QUICK_DISK
	uint32_t i;
	for (i = 0; i < len; i++)
	{
		buf[i] = read_byte(offset + i);
	}
#else
	uint8_t *disk=(uint8_t*)QDISK_BASE;
	memcpy(buf, disk+offset, len);
#endif
}

void ide_write(uint8_t *buf, uint32_t offset, uint32_t len)
{
#ifndef QUICK_DISK
	uint32_t i;
	for (i = 0; i < len; i++)
	{
		write_byte(offset + i, buf[i]);
	}
#else
	uint8_t *disk=(uint8_t*)QDISK_BASE;
	memcpy(disk+offset, buf, len);
#endif
}

static void
ide_writeback(void)
{
	static uint32_t counter = 0;
	counter++;
	if (counter == WRITEBACK_TIME * HZ)
	{
		cache_writeback();
		counter = 0;
	}
}

static volatile int has_ide_intr;

static void
ide_intr(void)
{
	has_ide_intr = 1;
}

void clear_ide_intr(void)
{
	has_ide_intr = 0;
}

void wait_ide_intr(void)
{
	while (has_ide_intr == 0)
	{
		wait_intr();
	}

	clear_ide_intr();
}

void init_ide(void)
{
	cache_init();
	add_irq_handle(0, ide_writeback);
	add_irq_handle(14, ide_intr);
}
