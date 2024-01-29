#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/operand.h"
/*
Put the implementations of `not' instructions here.
*/

static inline void instr_execute_1op(){
  operand_read(&opr_src);
  opr_src.val=~opr_src.val;
  operand_write(&opr_src);
}

make_instr_impl_1op(not, rm, b);
make_instr_impl_1op(not, rm, v);
