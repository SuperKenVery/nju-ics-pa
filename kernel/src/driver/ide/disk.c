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

void disk_do_read(void *buf, uint32_t sector)
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
	printk("kernel disk_do_read sec=%d\n",sector);
	for (i = 0; i < 512 / sizeof(uint32_t); i++)
	{
		uint32_t data = in_long(IDE_PORT_BASE);
		printk("kernel disk_do_read: got 0x%x\n", data);
		*(((uint32_t *)buf) + i) = data;
		int x=i*4;
		// Wanted: 0x103a, 0x103b, offset 0x3a, 0x3b
		if(x>=0x30 && x<=0x3f){
			printk("kernel disk_do_read inside if: got 0x%x\n", data);
			uint32_t _data=*(((uint32_t *)buf) + i);
			uint8_t *buf=(uint8_t*)&_data;
			for(int j=0;j<4;j++){
				printk("kernel disk_do_read: 0x%x -> 0x%x\n",512*8+x+j,buf[j]);
			}
		}
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
