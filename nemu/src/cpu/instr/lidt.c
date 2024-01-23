#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/modrm.h"
#include "cpu/operand.h"
#include "memory/memory.h"
/*
Put the implementations of `lidt' instructions here.
*/

make_instr_func(lidt) {
  int len=2;
  u16 size=instr_fetch(eip+len, 2); len+=2;
  u32 offset=instr_fetch(eip+len, 4); len+=4;

  cpu.idtr.size=size;
  cpu.idtr.offset=offset;

  print_asm_0("lidt", "", len);

  return len;
}
