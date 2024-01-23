#include "cpu/instr.h"
#include "cpu/instr_helper.h"
/*
Put the implementations of `sti' instructions here.
*/

make_instr_func(sti){
  int len=1;
  cpu.eflags.IF=1;

  return len;
}
