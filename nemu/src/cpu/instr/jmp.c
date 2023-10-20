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

        cpu.eip += rel.val;

        print_asm_1("jmp","",len,&rel);

        return len;
}
