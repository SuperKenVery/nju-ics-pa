#ifndef __CACHE_H__
#define __CACHE_H__

#include "nemu.h"

#ifdef CACHE_ENABLED

// init the cache
void init_cache();

// Write data to memory, with cache
void cached_write(paddr_t paddr, size_t len, uint32_t data);

// Read data from memory, with cache
uint32_t cached_read(paddr_t paddr, size_t len);

#endif

#endif
