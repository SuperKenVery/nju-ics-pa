#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/modrm.h"
#include "cpu/operand.h"
#include "memory/memory.h"
/*
Put the implementations of `lidt' instructions here.
*/

make_instr_func(lidt) {
  int len=1;
  OPERAND mem;
  mem.data_size=4;
  len+=modrm_rm(eip+len, &mem);
  print_asm_1("lidt", "", len, &mem);

  u16 limit=instr_fetch(mem.addr, 2);
  u32 offset=instr_fetch(mem.addr+2, 4);

  cpu.idtr.limit=limit;
  cpu.idtr.base=offset;


  return len;
}
