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
  OPERAND cw,stack,esp;
  cw.type=OPR_IMM;
  cw.data_size=data_size;
  cw.addr=eip+len;
  len+=cw.data_size/8;
  operand_read(&cw);

  esp.type=OPR_REG;
  esp.data_size=32;
  esp.addr=REG_ESP;
  operand_read(&esp);
  esp.val-=data_size/8;
  operand_write(&esp);

  stack.type=OPR_MEM;
  stack.data_size=data_size;
  stack.addr=esp.val;
  stack.val=cpu.eip;
  operand_write(&stack);

  if(data_size==16){
    cpu.eip=(cpu.eip+cw.val)&0xFFFF;
  }else{
    cpu.eip=cpu.eip+cw.val;
  }

  print_asm_1("call","",len,&cw);

  return len;
}
