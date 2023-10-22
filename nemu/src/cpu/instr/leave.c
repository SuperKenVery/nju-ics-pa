#include "cpu-ref/instr_ref.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/operand.h"
#include "cpu/reg.h"
/*
Put the implementations of `leave' instructions here.
*/

make_instr_func(leave_c9) {
  int len=1;
  OPERAND sp,bp,stack;

  // Stack AddrSize should always be 32 in nemu
  sp.data_size=32;
  sp.type=OPR_REG;
  sp.addr=REG_SP;
  bp.data_size=32;
  bp.type=OPR_REG;
  bp.addr=REG_BP;
  operand_read(&bp);
  sp.val=bp.val;
  operand_write(&sp);

  stack.data_size=data_size;
  stack.type=OPR_MEM;
  stack.addr=sp.val;
  operand_read(&stack);
  bp.data_size=data_size;
  bp.val=stack.val;
  operand_write(&bp);

  sp.val+=stack.data_size/8;
  operand_write(&sp);

  print_asm_0("leave","",len);

  return len;
}
