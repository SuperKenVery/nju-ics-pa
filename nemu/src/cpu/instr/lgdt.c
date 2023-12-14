#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/modrm.h"
#include "cpu/operand.h"
#include "memory/memory.h"
#include <assert.h>
/*
Put the implementations of `lgdt' instructions here.
*/

make_instr_func(lgdt) {
  OPERAND mem;
  int len=1;
  len+=modrm_rm(eip+len, &mem);
  assert(mem.type==OPR_MEM);

  u16 limit=laddr_read(mem.addr, 2);
  u32 base=laddr_read(mem.addr+2, 4);
  cpu.gdtr.base=base;
  cpu.gdtr.limit=limit;
  printf("LGDT: base=0x%x, limit=0x%x\n",base,limit);

  print_asm_1("lgdt", "", len, &mem);

  return len;
}
