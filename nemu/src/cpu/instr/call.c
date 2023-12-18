#include "cpu-ref/instr_ref.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/operand.h"
#include "cpu/reg.h"
/*
Put the implementations of `call' instructions here.
*/

make_instr_func(call_e8) {
  int len=1;
  OPERAND cw;
  cw.type=OPR_IMM;
  cw.data_size=data_size;
  cw.addr=eip+len;
  len+=cw.data_size/8;
  operand_read(&cw);

  push(cpu.eip+len,data_size);

  print_asm_1("call","",len,&cw);

  if(data_size==16){
    cpu.eip=(cpu.eip+cw.val)&0xFFFF;
  }else{
    cpu.eip=cpu.eip+cw.val;
  }

  // Displacement is relative to next instruction
  return len;
}
