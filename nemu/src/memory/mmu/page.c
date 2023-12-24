#include "cpu/cpu.h"
#include "memory/memory.h"
#include "memory/mmu/page.h"

// translate from linear address to physical address
paddr_t page_translate(laddr_t laddr)
{
#ifndef TLB_ENABLED
	CR3 *cr=&cpu.cr3;
	u32 page_directory_base=cr->pdbr;
	laddr_parse_t addr;
	addr.laddr=laddr;

	// First level
	u32 PDE_addr=page_directory_base+addr.page_directory_index*(sizeof(PDE));
	PDE pde;
	pde.val=paddr_read(PDE_addr,4);
	assert(pde.present==1);

	// Second level
	u32 PTE_addr=pde.page_frame+addr.page_entry_index*(sizeof(PTE));
	PTE pte;
	pte.val=paddr_read(PTE_addr, 4);
	assert(pte.present==1);

	return (pte.page_frame<<12)+addr.offset;
#else
	return tlb_read(laddr) | (laddr & PAGE_MASK);
#endif
}
