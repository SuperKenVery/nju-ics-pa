#include "cpu-ref/instr_ref.h"
#include "cpu/alu.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/operand.h"
#include "cpu/reg.h"
#include "memory/memory.h"
#include "memory/mmu/segment.h"
#include <assert.h>
#include <debug.h>

make_instr_func(jmp_near)
{
        OPERAND rel;
        rel.type = OPR_IMM;
        rel.sreg = SREG_CS;
        rel.data_size = data_size;
        rel.addr = eip + 1;

        operand_read(&rel);

        int offset = sign_ext(rel.val, data_size);
        // thank Ting Xu from CS'17 for finding this bug
        print_asm_1("jmp", "", 1 + data_size / 8, &rel);

        cpu.eip += offset;

        return 1 + data_size / 8;
}

make_instr_func(jmp_eb) {
        // Jump short
        int len=1;
        OPERAND rel;
        rel.type=OPR_IMM;
        rel.data_size=8;
        rel.addr=eip+len;
        len+=rel.data_size/8;

        operand_read(&rel);

        print_asm_1("jmp","",len,&rel);

        cpu.eip += sign_ext(rel.val, 8);

        return len;
}

// FF
make_instr_func(jmp_near_indirect){
        int len=1;
        OPERAND rm;
        rm.data_size=data_size;
        len+=modrm_rm(eip+len,&rm);
        operand_read(&rm);

        print_asm_1("jmp", "", len, &rm);
        cpu.eip=rm.val;
        
        // Don't add anything to ip anymore
        return 0;
}

// EA
make_instr_func(jmp_far) {
        int len=1;
        u32 offset=instr_fetch(eip+1, 4);
        u16 selector=instr_fetch(eip+1+4, 2);
        len+=(4+2);

        if(cpu.cr0.pe==0 || (cpu.cr0.pe==1 && cpu.eflags.VM==1)){
                // Real mode or Virtual 8086 mode
                cpu.cs.val=selector;
                cpu.eip=offset;
                load_sreg(SREG_CS);
        }

        if(cpu.cr0.pe==1 && cpu.eflags.VM==0) {
                // Protected mode

                // Call gate, task gate and task state segments are not implemented.
                SegDesc desc=get_seg_desc(selector);
                {
                        printf("Segment selector is %d\n",selector);
                        printf("Limit is 0x%x, base is 0x%x\n",get_limit(&desc),get_base(&desc));
                        hexdump(desc);
                }
                // We don't do priviledge checking...
                // u8 conforming=desc.type & (1<<2);
                // TODO: Call exception handler
                assert(desc.present);
                assert(offset<get_limit(&desc));
                cpu.cs.val=selector;
                cpu.eip=offset;
                load_sreg(SREG_CS);
        }

        // Don't modify eip anymore
        return 0;
}
