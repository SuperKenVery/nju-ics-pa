#include "common.h"
#include "memory.h"
#include "x86/memory.h"
#include "debug.h"
#include <string.h>

#define VMEM_ADDR 0xa0000
#define SCR_SIZE (320 * 200)
#define NR_PT ((SCR_SIZE + PT_SIZE - 1) / PT_SIZE) // number of page tables to cover the vmem

void video_mapping_write_test();
void video_mapping_read_test();

typedef union{
	uint32_t addr;
	struct {
		uint32_t offset: 12;
		uint32_t page_entry_index: 10;
		uint32_t page_directory_index: 10;
	};
} memaddr;

PDE *get_updir();

PTE new_table[NR_PTE] align_to_page;

void create_video_mapping()
{

	/* TODO: create an identical mapping from virtual memory area
	 * [0xa0000, 0xa0000 + SCR_SIZE) to physical memeory area
	 * [0xa0000, 0xa0000 + SCR_SIZE) for user program. You may define
	 * some page tables to create this mapping.
	 */

	Log("Creating video mapping");
	memaddr start,end;
	start.addr=0xa0000;
	end.addr=0xa0000+SCR_SIZE;
	assert(start.page_directory_index==end.page_directory_index);
	int page_directory_entry=start.page_directory_index;
	assert(page_directory_entry==(page_directory_entry&0x3ff));
	
	PDE *updir=get_updir();
	if(!updir[page_directory_entry].present){
		for(int i=0;i<NR_PTE;i++) new_table[i].present=0;
		assert((((uint32_t)new_table)&0xfff)==0);  // Really aligns to page
		updir[page_directory_entry].page_frame=((uint32_t)new_table)>>12;
		updir[page_directory_entry].present=1;
	}

	PTE* table=(PTE*)((updir[page_directory_entry].page_frame)<<12);
	Log("Before for");
	for(int page_table_entry=start.page_entry_index;page_table_entry<=end.page_entry_index;page_table_entry++){
		assert(page_table_entry==(page_table_entry&0x3ff));
		Log("[0x%x 0x%x 0x%x]",start.page_entry_index,page_table_entry,end.page_entry_index)
		table[page_table_entry].page_frame=(page_directory_entry << 10)|page_table_entry;
		table[page_directory_entry].present=1;
	}
	Log("Done video mapping");

}

void video_mapping_write_test()
{
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for (i = 0; i < SCR_SIZE / 4; i++)
	{
		buf[i] = i;
	}
	Log("vram writing test done");
}

void video_mapping_read_test()
{
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for (i = 0; i < SCR_SIZE / 4; i++)
	{
		assert(buf[i] == i);
	}
}

void video_mapping_clear()
{
	memset((void *)VMEM_ADDR, 0, SCR_SIZE);
}
