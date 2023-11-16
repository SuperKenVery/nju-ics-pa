#include "memory/mmu/cache.h"
#include "nemu.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "device/mm_io.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t hw_mem[MEM_SIZE_B];

uint32_t hw_mem_read(paddr_t paddr, size_t len)
{
	uint32_t ret = 0;
	memcpy(&ret, hw_mem + paddr, len);
	return ret;
}

void hw_mem_write(paddr_t paddr, size_t len, uint32_t data)
{
	if(paddr<0 || paddr>=MEM_SIZE_B) {
		printf("Invalid memory write operation: write at %u\n",paddr);
		exit(100);
	}
	if(paddr<=0x37570 && paddr+len>=0x37570){
		printf("hwmem write 37570\n");
	}
	memcpy(hw_mem + paddr, &data, len);
}

uint32_t paddr_read(paddr_t paddr, size_t len)
{
	uint32_t ret = 0;
#ifndef CACHE_ENABLED
	ret = hw_mem_read(paddr, len);
#else
	ret = cached_read(paddr, len);
#endif
	return ret;
}

void paddr_write(paddr_t paddr, size_t len, uint32_t data)
{
#ifndef CACHE_ENABLED
	hw_mem_write(paddr, len, data);
#else
	cached_write(paddr, len, data);
#endif
}

uint32_t laddr_read(laddr_t laddr, size_t len)
{
	return paddr_read(laddr, len);
}

void laddr_write(laddr_t laddr, size_t len, uint32_t data)
{
	paddr_write(laddr, len, data);
}

uint32_t vaddr_read(vaddr_t vaddr, uint8_t sreg, size_t len)
{
	assert(len == 1 || len == 2 || len == 4);
	return laddr_read(vaddr, len);
}

void vaddr_write(vaddr_t vaddr, uint8_t sreg, size_t len, uint32_t data)
{
	assert(len == 1 || len == 2 || len == 4);
	laddr_write(vaddr, len, data);
}

void init_mem()
{
	// clear the memory on initiation
	memset(hw_mem, 0, MEM_SIZE_B);
#ifdef CACHE_ENABLED
	init_cache();
#endif

#ifdef TLB_ENABLED
	make_all_tlb();
	init_all_tlb();
#endif
}

uint32_t instr_fetch(vaddr_t vaddr, size_t len)
{
	assert(len == 1 || len == 2 || len == 4);
	return vaddr_read(vaddr, SREG_CS, len);
}

uint8_t *get_mem_addr()
{
	return hw_mem;
}
