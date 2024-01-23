#include "cpu/intr.h"
#include "cpu/instr.h"
#include "cpu/instr/push.h"
#include "memory/memory.h"
#include "nemu.h"
#include <assert.h>


void raise_intr(uint8_t intr_no)
{
#ifdef IA32_INTR
	printf("INT 0x%x\n",intr_no);
	push(cpu.eflags.val, 32);
	push(cpu.cs.val, 32);
	push(cpu.eip,32);

	assert(intr_no<cpu.idtr.limit);

	u32 addr=cpu.idtr.base+intr_no*sizeof(GateDesc);
	GateDesc desc;
	desc.val[0]=laddr_read(addr, 4);
	desc.val[1]=laddr_read(addr+4, 4);

	printf("IRQ Handler at 0x%x%x\n",desc.offset_31_16,desc.offset_15_0);

	if(desc.type==0x5){
		printf("Interrupt to task gate is not implemented. \n");
		assert(desc.type!=0x5);
	}else if(desc.type==0x6 || desc.type==0x7){
		printf("16-bit interrupt/trap is not implemented. \n");
		assert(0);
	}
	
	if(desc.type==0xe) cpu.eflags.IF=0; // Interrupt gate

	cpu.cs.val=desc.selector;
	cpu.eip=(desc.offset_31_16<<16)|(desc.offset_15_0);
#endif
}

void raise_sw_intr(uint8_t intr_no)
{
	// return address is the next instruction
	cpu.eip += 2;
	raise_intr(intr_no);
}
