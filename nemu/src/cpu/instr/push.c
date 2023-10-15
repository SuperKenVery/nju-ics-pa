#include "cpu-ref/instr_ref.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/operand.h"
#include "cpu/reg.h"
/*
Put the implementations of `push' instructions here.
*/

make_instr_func(push_50) {
  OPERAND r,stack,sp;
  r.data_size=data_size;
  r.type=OPR_REG;
  r.addr=opcode%8;
  operand_read(&r);

  sp.data_size=data_size;
  sp.type=OPR_REG;
  sp.addr=REG_SP;
  operand_read(&sp);
  sp.val-=data_size/8;
  operand_write(&sp);

  stack.data_size=data_size;
  stack.type=OPR_MEM;
  stack.addr=sp.val;
  stack.val=r.val; 
  operand_write(&stack); 

  return 1;
}
