#include "cpu/cpu.h"
#include "cpu/reg.h"
#include "debug.h"
#include "memory/memory.h"
#include "memory/mmu/segment.h"
#include <assert.h>

u32 get_base(SegDesc *desc) {
	u32 base=0;
	base|=desc->base_15_0;
	base|=desc->base_23_16<<16;
	base|=desc->base_31_24<<24;

	return base;
}

u32 get_limit(SegDesc *desc) {
	u32 limit=0;
	limit|=desc->limit_15_0;
	limit|=desc->limit_19_16<<16;

	return limit;
}

u32 get_type(SegDesc *desc) {
	u32 type=0;
	type|=desc->type;
	type|=desc->segment_type<<4;

	return type;
}

// return the linear address from the virtual address and segment selector
uint32_t segment_translate(uint32_t offset, uint8_t sreg)
{
	/* TODO: perform segment translation from virtual address to linear address
	 * by reading the invisible part of the segment register 'sreg'
	 */
	SegReg *segreg=&cpu.segReg[sreg];
	
	return segreg->base+offset;
}

SegDesc get_seg_desc(int index){
	u32 gdt_addr=cpu.gdtr.base;
	if(index>=cpu.gdtr.limit){
		printf("NEMU: Segment selector index out of range: limit is %d, requested %d.\n",
			cpu.gdtr.limit, index);
	}
	u32 desc_addr=gdt_addr+index*sizeof(SegDesc);
	SegDesc desc;
	for(int i=0;i<sizeof(desc)/sizeof(u32);i++){
		*((u32*)&desc+i)=laddr_read(desc_addr+i, 4);
	}
	return desc;
}

// load the invisible part of a segment register
void load_sreg(uint8_t sreg)
{
	/* TODO: load the invisibile part of the segment register 'sreg' by reading the GDT.
	 * The visible part of 'sreg' should be assigned by mov or ljmp already.
	 */
	assert(sreg<=5);
	printf("sreg index: %d\n",sreg);
	SegReg *reg=&cpu.segReg[sreg];
	u32 gdt_addr=cpu.gdtr.base;
	u32 index=reg->index;
	printf("gdt index is %d\n", index);
	if(index>=cpu.gdtr.limit){
		printf("NEMU: Segment selector index out of range: limit is %d, requested %d.\n",
			cpu.gdtr.limit, index);
	}
	u32 desc_addr=gdt_addr+index*sizeof(SegDesc);
	hexdump_pointer((hw_mem+desc_addr), 8);
	SegDesc desc;
	for(int i=0;i<sizeof(desc)/sizeof(u32);i++){
		u32 a=laddr_read(desc_addr+i, 4);
		hexdump(a);
		*(((u32*)&desc)+(i*sizeof(u32)))=laddr_read(desc_addr+i, 4);
	}
	hexdump(desc);

	// Checks under flat mode
	{
		assert(sizeof(SegDesc)==8);
		assert(desc.base_15_0==0);
		assert(desc.base_23_16==0);
		assert(desc.base_31_24==0);
		assert(desc.limit_15_0==((1<<16)-1));
		assert(desc.limit_19_16==((1<<4)-1));
		assert(desc.granularity==1);
	}

	reg->base=get_base(&desc);
	reg->limit=get_limit(&desc);
	reg->type=get_type(&desc);
	reg->privilege_level=desc.privilege_level;
	reg->soft_use=desc.soft_use;
}
