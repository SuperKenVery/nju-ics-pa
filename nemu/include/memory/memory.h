#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "nemu.h"
#include "memory/mmu/cache.h"
#include "memory/mmu/tlb.h"
#include "memory/mmu/segment.h"
#include "memory/mmu/page.h"
#include <stdint.h>

// 128MB memory
#define MEM_SIZE_B 128 * 1024 * 1024

#ifdef QUICK_DISK
const uint32_t QDISK_BASE=((uint32_t)3300)*1024*1024;
#endif

extern uint8_t hw_mem[];
extern uint64_t hw_mem_access_time;

#define hwa_to_va(p) ((void *)(hw_mem + (unsigned)p))

// Write data to hardware memory
void hw_mem_write(paddr_t paddr, size_t len, uint32_t data);

// Read data from hardware memory
uint32_t hw_mem_read(paddr_t paddr, size_t len);

// read memory with physical address and size of bytes
uint32_t paddr_read(paddr_t paddr, size_t len);

// write data to memory with physical address and size of bytes
void paddr_write(paddr_t paddr, size_t len, uint32_t data);

// read memory with linear address and size of bytes
uint32_t laddr_read(laddr_t laddr, size_t len);

// write data to memory with linear address and size of bytes
void laddr_write(laddr_t laddr, size_t len, uint32_t data);

// read memory with virtual address and size of bytes
uint32_t vaddr_read(vaddr_t vaddr, uint8_t sreg, size_t len);

// write data to memory with virtual address and size of bytes
void vaddr_write(vaddr_t vaddr, uint8_t sreg, size_t len, uint32_t data);

// initialize the memory by clearing it
void init_mem();

// fetch an instruction
uint32_t instr_fetch(vaddr_t vaddr, size_t len);

// print memory start from physical address addr and size len bytes
//void mem_print(uint32_t addr, size_t len);
uint8_t *get_mem_addr();

#endif
