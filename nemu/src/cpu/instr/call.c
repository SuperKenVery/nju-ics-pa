#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/operand.h"
/*
Put the implementations of `call' instructions here.
*/

make_instr_func(call_e8) {
  int len=1;
  OPERAND cw;
  cw.type=OPR_IMM;
  cw.data_size=16;
  cw.addr=eip+len;
  len+=cw.data_size/8;

  operand_read(&cw);
  cpu.eip=(cpu.eip+cw.val)&0xFFFF;

  return len;
}
