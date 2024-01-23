#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include <assert.h>
/*
Put the implementations of `iret' instructions here.
*/

make_instr_func(iret){
  assert(cpu.cr0.pe);
  cpu.eip=pop(32);
  cpu.cs.val=pop(32);
  cpu.eflags.val=pop(32);

  return 0;
}
