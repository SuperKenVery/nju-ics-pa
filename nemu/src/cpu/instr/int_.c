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
  return 1;
}

make_instr_func(int_imm){
  int len=1;
  u8 imm=instr_fetch(eip+len, 1); len++;
  raise_intr(imm);

  return len;
}

make_instr_func(into){
  if(cpu.eflags.OF){
    raise_intr(4);
  }
  return 1;
}
