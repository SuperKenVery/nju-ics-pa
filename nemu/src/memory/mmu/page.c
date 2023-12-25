#include "cpu/cpu.h"
#include "memory/memory.h"
#include "memory/mmu/page.h"

void debug_print_page_tables(laddr_t laddr){
	CR3 *cr=&cpu.cr3;
	u32 page_directory_base=cr->pdbr;
	printf("eip=0x%x Read page_directory_base=0x%x\n", cpu.eip, page_directory_base);
	laddr_parse_t addr;
	addr.laddr=laddr;

	printf("Accessing addr: 0x%x %x %x\n",addr.page_directory_index,addr.page_entry_index,addr.offset);

	// First level
	printf("Page directory entries: \n");
	for(int i=0;i<NR_PDE;i++){
		u32 pde_addr=page_directory_base+i*sizeof(PDE);
		PDE pde;
		pde.val=paddr_read(pde_addr, 4);
		if(pde.present==0) continue;
		printf("0x%x -> [0x%x] 0x%x", pde_addr, i, pde.page_frame<<12);
		if(i==addr.page_directory_index) printf(" [ <- ] ");
		printf("\n");
	}
	printf("\n\n");

	u32 PDE_addr=page_directory_base+addr.page_directory_index*(sizeof(PDE));
	PDE pde;
	pde.val=paddr_read(PDE_addr,4);

	// Second level
	printf("Page table entries: \n");
	for(int i=0;i<NR_PTE;i++){
		u32 pte_addr=pde.page_frame+i*sizeof(PTE);
		PTE pte;
		pte.val=paddr_read(pte_addr, 4);
		if(pte.present==0) continue;
		printf("0x%x -> [0x%x] 0x%x", pte_addr, i, pte.page_frame<<12);
		if(i==addr.page_entry_index) printf(" [ <- ] ");
		printf("\n");
	}
	printf("\n\n");
}

// translate from linear address to physical address
paddr_t page_translate(laddr_t laddr)
{
#ifndef TLB_ENABLED
	// Only print once
	static int print_count=0;
	if(print_count==0){
		debug_print_page_tables(laddr);
		print_count++;
	}
	CR3 *cr=&cpu.cr3;
	u32 page_directory_base=cr->pdbr;
	laddr_parse_t addr;
	addr.laddr=laddr;

	// First level
	u32 PDE_addr=(page_directory_base<<12)+addr.page_directory_index*(sizeof(PDE));
	PDE pde;
	pde.val=paddr_read(PDE_addr,4);
	assert(pde.present==1);

	// Second level
	u32 PTE_addr=(pde.page_frame<<12)+addr.page_entry_index*(sizeof(PTE));
	PTE pte;
	pte.val=paddr_read(PTE_addr, 4);
	assert(pte.present==1);

	return (pte.page_frame<<12)+addr.offset;
#else
	return tlb_read(laddr) | (laddr & PAGE_MASK);
#endif
}
