#include "cpu-ref/instr_ref.h"
#include "cpu/alu.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/operand.h"

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
        printf("eip: 0x%x\tEnter FF\t37570 is %x %x %x %x\n",cpu.eip,hw_mem[0x37570],hw_mem[0x37570+1],hw_mem[0x37570+2],hw_mem[0x37570+3]);
        int len=1;
        OPERAND rm;
        rm.data_size=data_size;
        len+=modrm_rm(eip+len,&rm);
        printf("eip: 0x%x\tAfter modrm\t37570 is %x %x %x %x\n",cpu.eip,hw_mem[0x37570],hw_mem[0x37570+1],hw_mem[0x37570+2],hw_mem[0x37570+3]);
        operand_read(&rm);
        printf("eip: 0x%x\tAfter operand read\t37570 is %x %x %x %x\n",cpu.eip,hw_mem[0x37570],hw_mem[0x37570+1],hw_mem[0x37570+2],hw_mem[0x37570+3]);

        print_asm_1("jmp", "", len, &rm);
        cpu.eip=rm.val;
        
        // Don't add anything to ip anymore
        return 0;
}
