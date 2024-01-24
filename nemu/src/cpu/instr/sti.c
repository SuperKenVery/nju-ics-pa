#include "cpu/instr.h"
#include "cpu/instr_helper.h"
/*
Put the implementations of `sti' instructions here.
*/

make_instr_func(sti){
  printf("sti: eip=0x%x\n",eip);
  cpu.eflags.IF=1;

  printf("sti: eip became 0x%x\n",eip);
  return 1;
}
