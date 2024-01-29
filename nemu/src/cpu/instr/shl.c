#include "cpu-ref/instr_ref.h"
#include "cpu/instr.h"
#include "cpu/operand.h"
#include "cpu/reg.h"

static inline void instr_execute_2op() 
{
	operand_read(&opr_src);
	operand_read(&opr_dest);
	// printf("Shifting 0x%x left %d digits\n",opr_dest.val,opr_src.val);
	opr_dest.val = alu_shl(opr_src.val, opr_dest.val, opr_dest.data_size);
	operand_write(&opr_dest);
}

make_instr_impl_2op(shl, i, rm, b)
make_instr_impl_2op(shl, i, rm, bv)
make_instr_impl_2op(shl, c, rm, b)
make_instr_impl_2op(shl, c, rm, bv)

static inline void instr_execute_1op() 
{
	operand_read(&opr_src);
	opr_src.val = alu_shl(1, opr_src.val, opr_src.data_size);
	operand_write(&opr_src);
}

make_instr_impl_1op(shl, rm, b)
make_instr_impl_1op(shl, rm, v)
