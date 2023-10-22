#include "cpu-ref/instr_ref.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/operand.h"
#include "cpu/reg.h"
/*
Put the implementations of `push' instructions here.
*/

void do_push(u32 data, int size) {
  OPERAND stack,esp;

  esp.type=OPR_REG;
  esp.data_size=32;
  esp.addr=REG_ESP;
  operand_read(&esp);
  esp.val-=size/8;
  operand_write(&esp);

  stack.type=OPR_MEM;
  stack.data_size=size;
  stack.addr=esp.val;
  stack.val=data;
  operand_write(&stack);
}

make_instr_func(push_ff) {
  int len=1;
  OPERAND m;

  m.data_size=data_size;
  len+=modrm_rm(eip+len,&m);
  assert(m.type==OPR_MEM);
  operand_read(&m);

  do_push(m.val, m.data_size);

  print_asm_1("push", "", len, &m);

  return len;
}

make_instr_func(push_50) {
  OPERAND r;
  r.data_size=data_size;
  r.type=OPR_REG;
  r.addr=opcode%8;
  operand_read(&r);

  do_push(r.val, r.data_size);

  print_asm_1("push", "", 1, &r);

  return 1;
}

make_instr_func(push_6a) {
  int len=1;
  OPERAND imm;

  imm.data_size=8;
  imm.type=OPR_IMM;
  imm.addr=eip+len;
  len+=imm.data_size/8;
  operand_read(&imm);

  do_push(imm.val, imm.data_size);

  print_asm_1("push", "", len, &imm);

  return len;
}

make_instr_func(push_68) {
  int len=1;
  OPERAND imm;

  imm.data_size=data_size;
  imm.type=OPR_IMM;
  imm.addr=eip+len;
  len+=imm.data_size/8;
  operand_read(&imm);

  do_push(imm.val, imm.data_size);

  print_asm_1("push", "", len, &imm);

  return len;
}
