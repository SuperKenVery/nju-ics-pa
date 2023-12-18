#include "cpu-ref/instr_ref.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/modrm.h"
#include "cpu/operand.h"
#include "cpu/reg.h"
#include "device/port_io.h"
#include <assert.h>
/*
Put the implementations of `pop' instructions here.
*/

u32 pop(int size) {
  OPERAND esp,stack;

  esp.type=OPR_REG;
  esp.data_size=32;
  esp.addr=REG_ESP;
  operand_read(&esp);

  stack.type=OPR_MEM;
  stack.sreg=SREG_SS;
  stack.data_size=size;
  stack.addr=esp.val;
  operand_read(&stack);

  esp.val+=stack.data_size/8;
  operand_write(&esp);

  return stack.val;
}

make_instr_func(pop_8f) {
  int len=1;
  OPERAND m;
  m.data_size=data_size;
  len+=modrm_rm(eip+len, &m);
  assert(m.type==OPR_MEM);

  m.val=pop(m.data_size);
  operand_write(&m);

  print_asm_1("pop", "", len, &m);

  return len;
}

make_instr_func(pop_58) {
  int len=1;
  OPERAND r;

  r.type=OPR_REG;
  r.data_size=data_size;
  r.addr=opcode%8;
  
  r.val=pop(r.data_size);
  operand_write(&r);

  print_asm_1("pop", "", len, &r);

  return len;
}
