#include "memory/mmu/cache.h"
#include "cpu/cpu.h"
#include "memory.h"
#include "memory/memory.h"
#include "nemu.h"
#include "debug.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define CACHE_BLOCK_SIZE 64
#define CACHE_SIZE 64*1024
#define GRP_SIZE 8
#define GRP_NUM CACHE_SIZE/(GRP_SIZE*CACHE_BLOCK_SIZE)

// 1024 groups, 10 bits
// MemAddr:  16bit_mark   10bit_group_num   6bit_block_offset

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

memaddr memaddr_load(paddr_t addr){
	uaddr u_address;
	u_address.paddr=addr;
	return u_address.maddr;
}

typedef struct{
	u8 valid;
	u16 mark;
	char content[CACHE_BLOCK_SIZE];
} cache_block;

void cache_block_init(cache_block *this){
	this->valid=false;
}

// Load a block of memory into cache
void cache_block_load(cache_block *this, paddr_t mem_addr){
	// Make sure we're operating on start of a block
	u32 mask=~0b111111;
	mem_addr=mem_addr&mask;
	assert((mem_addr&0b111111)==0);

	this->valid=true;
	memaddr addr=memaddr_load(mem_addr);
	this->mark=addr.mark;
	memcpy(this->content, &hw_mem[mem_addr], CACHE_BLOCK_SIZE);
}

// Read from cache
u32 cache_block_read(cache_block *this, u32 offset, size_t len){
	u8 valid=(offset>=0 && offset+len<=CACHE_BLOCK_SIZE);
	if(valid==false){
		printf("Cache block read: Invalid arguments\n");
		printf("offset=%d len=%d\n",offset,len);
	}
	assert(offset>=0 && offset+len<=CACHE_BLOCK_SIZE);

	u32 ret=0;
	memcpy(&ret,&this->content[offset],len);
	return ret;
}

// Write to cache
void cache_block_write(cache_block *this, u32 offset, u32 data, size_t len){
	assert(offset>=0 && offset<CACHE_BLOCK_SIZE);
	char* dst=&this->content[offset];

	memcpy(dst, &data, len);
}

typedef struct{
	cache_block members[GRP_SIZE];
} cache_group;

void cache_group_init(cache_group *this){
	for(int i=0;i<GRP_SIZE;i++){
		cache_block *target=&this->members[i];
		cache_block_init(target);
	}
}

void cache_group_load(cache_group *this, paddr_t mem_addr){
	memaddr addr=memaddr_load(mem_addr);

	// Case 1 - addr already in cache
	for(int i=0;i<GRP_SIZE;i++){
		cache_block* target=&this->members[i];
		if(target->valid==true && target->mark==addr.mark){
			cache_block_load(target, mem_addr);
			return;
		}
	}

	// Case 2 - still empty slots
	for(int i=0;i<GRP_SIZE;i++){
		cache_block *target=&this->members[i];
		if(target->valid==false){
			cache_block_load(target, mem_addr);
			return;
		}
	}

	// Case 3 - fuck an existing cache block
	// pseudo random
	u32 target_index=cpu.eip*3%GRP_SIZE;
	cache_block *target=&this->members[target_index];
	cache_block_load(target, mem_addr);
}

u32 cache_group_read(cache_group *this, paddr_t mem_addr, size_t len){
	memaddr addr=memaddr_load(mem_addr);
	for(int i=0;i<GRP_SIZE;i++){
		cache_block *target=&this->members[i];
		if(target->valid==false) continue;

		if(target->mark==addr.mark)
			return cache_block_read(target, addr.offset, len);
	}

	// Not found!
	assert(false);
}

// Write to cache, if the addr is in cache
void cache_group_write(cache_group *this, paddr_t mem_addr, u32 data, size_t len){
	memaddr addr=memaddr_load(mem_addr);

	for(int i=0;i<GRP_SIZE;i++){
		cache_block *target=&this->members[i];
		if(target->valid && target->mark==addr.mark){
			cache_block_write(target, addr.offset, data, len);
			return;
		}
	}
}

u8 cache_group_has_cache(cache_group *this, paddr_t mem_addr){
	memaddr addr=memaddr_load(mem_addr);

	for(int i=0;i<GRP_SIZE;i++){
		cache_block *target=&this->members[i];
		if(target->valid==false) continue;
		if(target->mark==addr.mark) return 1;
	}

	return 0;
}

typedef struct{
	cache_group groups[GRP_NUM];
} cache;

void cache_init(cache *this){
	for(int i=0;i<GRP_NUM;i++){
		cache_group *target=&this->groups[i];
		cache_group_init(target);
	}
}

void cache_load(cache *this, paddr_t mem_addr){
	memaddr addr=memaddr_load(mem_addr);

	cache_group *grp=&this->groups[addr.group_idx];
	cache_group_load(grp, mem_addr);
}

u32 cache_read(cache *this, paddr_t mem_addr, size_t len){
	memaddr addr=memaddr_load(mem_addr);

	cache_group *target=&this->groups[addr.group_idx];
	return cache_group_read(target, mem_addr, len);
}

void cache_write(cache *this, paddr_t mem_addr, u32 data, size_t len){
	memaddr addr=memaddr_load(mem_addr);

	cache_group *group=&this->groups[addr.group_idx];
	cache_group_write(group, mem_addr, data, len);
}

typedef enum{
	covered,
	not_loaded,
	not_aligned
} cache_coverage;

cache_coverage cache_has_data(cache *this, paddr_t mem_addr, size_t len){
	memaddr addr=memaddr_load(mem_addr);

	cache_group *grp=&this->groups[addr.group_idx];
	if(addr.offset+len>CACHE_BLOCK_SIZE) return not_aligned;
	if(cache_group_has_cache(grp, mem_addr)==false) return not_loaded;
	return covered;
}

cache nemu_cache;

// init the cache
void init_cache()
{
	cache_init(&nemu_cache);
}

// write data to cache
void cached_write(paddr_t paddr, size_t len, uint32_t data)
{
	hw_mem_write(paddr, len, data);
	cache_write(&nemu_cache, paddr, data, len);
}

// read data from cache
uint32_t cached_read(paddr_t paddr, size_t len)
{
	cache_coverage coverage=cache_has_data(&nemu_cache, paddr, len);
	u32 result;
	if(coverage==not_aligned){
		result=hw_mem_read(paddr, len);
	}else{
		if(coverage==not_loaded)
			cache_load(&nemu_cache, paddr);
		result=cache_read(&nemu_cache, paddr, len);
	}


	u32 ground_truth=hw_mem_read(paddr, len);

	if(result!=ground_truth){
		printf("Cache read error! Reading %d bytes at 0x%x\n",len,paddr);
		printf("Result from memory: 0x%x\tResult from cache: 0x%x\n",ground_truth,result);
		memaddr mem=memaddr_load(paddr);
		printf("0x%x: offset=%d\tgroup_idx=%d\tmark=%d\n",paddr,mem.offset,mem.group_idx,mem.mark);
		u32 mask=~0b111111;
		hexdump_pointer(hw_mem+(paddr&mask),CACHE_BLOCK_SIZE);
		cache_group *grp=&nemu_cache.groups[mem.group_idx];
		for(int i=0;i<GRP_SIZE;i++){
			cache_block *target=&grp->members[i];
			if(target->mark==mem.mark){
				hexdump_pointer(target->content, CACHE_BLOCK_SIZE);
				break;
			}
		}
		assert(0);
	}
	
	return result;
}
