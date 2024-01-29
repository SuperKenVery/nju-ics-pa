#include "memory/mmu/cache.h"
#include "memory/mmu/page.h"
#include "memory/mmu/segment.h"
#include "nemu.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "device/mm_io.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t hw_mem[MEM_SIZE_B];

void pinvalid_addr(char *msg, u32 addr){
	printf("%s: \033[38;2;255;0;0mInvalid memory address \033[38;2;255;255;0m0x%x\033[0m at eip=0x%x\n",msg,addr,cpu.eip);
	// debug_print_page_tables(addr);
	assert(0);
}

uint32_t hw_mem_read(paddr_t paddr, size_t len)
{
	if(paddr<0 || paddr+len>=MEM_SIZE_B) {
		if(paddr<0) printf("<0 ");
		else printf(">= mem_size_b");
		pinvalid_addr("hw_mem_read" , paddr);
	}
	uint32_t ret = 0;
	memcpy(&ret, hw_mem + paddr, len);
	return ret;
}

void hw_mem_write(paddr_t paddr, size_t len, uint32_t data)
{
	if(paddr<0 || paddr+len>=MEM_SIZE_B) {
		pinvalid_addr("hw_mem_write", paddr);
	}
	memcpy(hw_mem + paddr, &data, len);
}

uint32_t paddr_read(paddr_t paddr, size_t len)
{
	if(paddr<0 || paddr+len>=MEM_SIZE_B) {
		pinvalid_addr("paddr_read", paddr);
	}
	int map_num=is_mmio(paddr);
	if(map_num==-1){
		uint32_t ret = 0;
#ifndef CACHE_ENABLED
		ret = hw_mem_read(paddr, len);
#else
		ret = cached_read(paddr, len);
#endif
		return ret;
	}else{
		return mmio_read(paddr, len, map_num);
	}
}

void paddr_write(paddr_t paddr, size_t len, uint32_t data)
{
	if(paddr<0 || paddr+len>=MEM_SIZE_B) {
		pinvalid_addr("paddr_write", paddr);
	}
	int map_num=is_mmio(paddr);
	if(map_num==-1){
#ifndef CACHE_ENABLED
		hw_mem_write(paddr, len, data);
#else
		cached_write(paddr, len, data);
#endif
	}else{
		mmio_write(paddr, len, data, map_num);
	}
}

uint32_t laddr_read(laddr_t laddr, size_t len)
{
	if(PAGE(laddr)!=PAGE(laddr+len-1)){
		// Cross page
		// low is both low addr and least significant
		int lowlen=NEXTPAGE(laddr)-laddr;
		u32 low=laddr_read(laddr, lowlen);
		u32 high=laddr_read(NEXTPAGE(laddr),len-lowlen);
		return (high<<(lowlen*8))|low;
	}else{
		paddr_t paddr;
		if(cpu.cr0.pg==1){
			paddr=page_translate(laddr);
		}else{
			paddr=laddr;
		}
		return paddr_read(paddr, len);
	}
}

void laddr_write(laddr_t laddr, size_t len, uint32_t data)
{
	if(PAGE(laddr)!=PAGE(laddr+len-1)){
		// Cross page
		int lowlen=NEXTPAGE(laddr)-laddr;
		laddr_write(laddr, lowlen, data&((1<<(lowlen*8))-1));
		laddr_write(NEXTPAGE(laddr), len-lowlen, data>>(lowlen*8));
	}else{
		paddr_t paddr;
		if(cpu.cr0.pg==1){
			paddr=page_translate(laddr);
		}else{
			paddr=laddr;
		}
		paddr_write(paddr, len, data);
	}
}

uint32_t vaddr_read(vaddr_t vaddr, uint8_t sreg, size_t len)
{
	assert(len == 1 || len == 2 || len == 4);
	laddr_t laddr;
	if(cpu.cr0.pe){
		laddr=segment_translate(vaddr, sreg);
	}else{
		laddr=vaddr;
	}
	return laddr_read(laddr, len);
}

void vaddr_write(vaddr_t vaddr, uint8_t sreg, size_t len, uint32_t data)
{
	assert(len == 1 || len == 2 || len == 4);
	laddr_t laddr;
	if(cpu.cr0.pe){
		laddr=segment_translate(vaddr, sreg);
	}else{
		laddr=vaddr;
	}
	laddr_write(laddr, len, data);
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
