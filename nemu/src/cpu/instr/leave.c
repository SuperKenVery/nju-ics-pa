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
  OPERAND sp,bp;

  // Stack AddrSize should always be 32 in nemu
  sp.data_size=32;
  sp.type=OPR_REG;
  sp.addr=REG_SP;

  bp.data_size=32;
  bp.type=OPR_REG;
  bp.addr=REG_BP;

  // sp=bp
  operand_read(&bp);
  sp.val=bp.val;
  operand_write(&sp);

  // bp=pop()
  bp.data_size=data_size;
  bp.val=pop(32);
  operand_write(&bp);

  print_asm_0("leave","",len);

  return len;
}
