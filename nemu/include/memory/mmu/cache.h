#ifndef __CACHE_H__
#define __CACHE_H__

#include "nemu.h"
#include <stddef.h>

#ifdef CACHE_ENABLED

// init the cache
void init_cache();

// Write data to memory, with cache
void cached_write(paddr_t paddr, size_t len, uint32_t data);

// Read data from memory, with cache
uint32_t cached_read(paddr_t paddr, size_t len);

#define CACHE_BLOCK_SIZE 64
#define CACHE_SIZE 64*1024
#define GRP_SIZE 8
#define GRP_NUM (CACHE_SIZE/(GRP_SIZE*CACHE_BLOCK_SIZE))

typedef struct{
	// Little endian, low digit comes in low address
	u32 offset:6;
	u32 group_idx:10;
	u32 mark:16;
} memaddr;

typedef union{
	paddr_t paddr;
	memaddr maddr;
} uaddr;

typedef struct{
	u8 valid;
	u16 mark;
	char content[CACHE_BLOCK_SIZE];
} cache_block;

typedef struct{
	cache_block members[GRP_SIZE];
} cache_group;

typedef struct{
	cache_group groups[GRP_NUM];
} cache;

typedef enum{
	covered,
	not_loaded,
	not_aligned
} cache_coverage;

memaddr memaddr_load(paddr_t addr);
void cache_block_init(cache_block *this);
void cache_block_load(cache_block *this, paddr_t mem_addr);
u32 cache_block_read(cache_block *this, u32 offset, size_t len);
void cache_block_write(cache_block *this, u32 offset, u32 data, size_t len);
void cache_group_init(cache_group *this);
void cache_group_load(cache_group *this, paddr_t mem_addr);
u32 cache_group_read(cache_group *this, paddr_t mem_addr, size_t len);
void cache_group_write(cache_group *this, paddr_t mem_addr, u32 data, size_t len);
u8 cache_group_has_cache(cache_group *this, paddr_t mem_addr);
void cache_init(cache *this);
void cache_load(cache *this, paddr_t mem_addr);
u32 cache_read(cache *this, paddr_t mem_addr, size_t len);
void cache_write(cache *this, paddr_t mem_addr, u32 data, size_t len);
cache_coverage cache_has_data(cache *this, paddr_t mem_addr, size_t len);



#endif

#endif
