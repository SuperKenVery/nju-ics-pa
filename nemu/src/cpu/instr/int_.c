#include "cpu/instr_helper.h"
#include "cpu/intr.h"
#include "cpu/instr.h"
#include "cpu/operand.h"
#include "memory/memory.h"

/*
Put the implementations of `int' instructions here.

Special note for `int': please use the instruction name `int_' instead of `int'.
*/

make_instr_func(int_3){
  raise_intr(3);

  print_asm_0("int", "3", 1);
  
  return 1;
}

make_instr_func(int_imm){
  int len=1;
  OPERAND imm;
  imm.data_size=8;
  imm.type=OPR_IMM;
  imm.addr=eip+len; len+=imm.data_size/8;
  operand_read(&imm);
  raise_sw_intr(imm.val);

  print_asm_1("int","",len,&imm);

  return 0;
}

make_instr_func(into){
  if(cpu.eflags.OF){
    raise_intr(4);
  }
  return 1;
}
