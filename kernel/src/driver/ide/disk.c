#include "common.h"
#include "debug.h"
#include "x86.h"
#include <stdint.h>

//#define USE_DMA_READ

#define IDE_PORT_BASE 0x1F0

void dma_prepare(void *);
void dma_issue_read(void);

void clear_ide_intr(void);
void wait_ide_intr(void);

static void waitdisk()
{
	while ((in_byte(IDE_PORT_BASE + 7) & (0x80 | 0x40)) != 0x40)
		;
}

static void
ide_prepare(uint32_t sector)
{
	waitdisk();

#ifdef USE_DMA_READ
	out_byte(IDE_PORT_BASE + 1, 1);
#else
	out_byte(IDE_PORT_BASE + 1, 0);
#endif

	out_byte(IDE_PORT_BASE + 2, 1);
	out_byte(IDE_PORT_BASE + 3, sector & 0xFF);
	out_byte(IDE_PORT_BASE + 4, (sector >> 8) & 0xFF);
	out_byte(IDE_PORT_BASE + 5, (sector >> 16) & 0xFF);
	out_byte(IDE_PORT_BASE + 6, 0xE0 | ((sector >> 24) & 0xFF));
}

static inline void
issue_read()
{
#ifdef USE_DMA_READ
	out_byte(IDE_PORT_BASE + 7, 0xc8);
	dma_issue_read();
#else
	out_byte(IDE_PORT_BASE + 7, 0x20);
#endif
}

static inline void
issue_write()
{
	out_byte(IDE_PORT_BASE + 7, 0x30);
}

void disk_do_read(volatile void *buf, uint32_t sector)
{
#ifdef USE_DMA_READ
	dma_prepare(buf);
#endif

	clear_ide_intr();

	ide_prepare(sector);
	issue_read();

	wait_ide_intr();

#ifndef USE_DMA_READ
	int i;
	volatile uint32_t *wbuf=(uint32_t*)buf;
	for (i = 0; i < 512 / sizeof(uint32_t); i++)
	{
		uint32_t data = in_long(IDE_PORT_BASE);
		wbuf[i] = data;
		if(wbuf[i]!=data){
			Log("Data corruption: 0x%x -> 0x%x",data,wbuf[i]);
		}
		assert(wbuf[i]==data);
	}

#endif
}

void disk_do_write(void *buf, uint32_t sector)
{
	int i;

	ide_prepare(sector);
	issue_write();

	for (i = 0; i < 512 / sizeof(uint32_t); i++)
	{
		out_long(IDE_PORT_BASE, *(((uint32_t *)buf) + i));
	}
}
