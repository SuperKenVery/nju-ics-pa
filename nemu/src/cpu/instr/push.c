#include "cpu-ref/instr_ref.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/operand.h"
#include "cpu/reg.h"
/*
Put the implementations of `push' instructions here.
*/

void push(u32 data, int size) {
  OPERAND stack,esp;

  esp.type=OPR_REG;
  esp.data_size=32;
  esp.addr=REG_ESP;
  operand_read(&esp);
  esp.val-=size/8;
  operand_write(&esp);

  stack.type=OPR_MEM;
  stack.sreg=SREG_SS;
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

  push(m.val, m.data_size);

  print_asm_1("push", "", len, &m);

  return len;
}

make_instr_func(push_50) {
  OPERAND r;
  r.data_size=data_size;
  r.type=OPR_REG;
  r.addr=opcode%8;
  operand_read(&r);

  push(r.val, r.data_size);

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

  push(imm.val, data_size);

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

  push(imm.val, imm.data_size);

  print_asm_1("push", "", len, &imm);

  return len;
}

#define PUSH_SEGREG(reg) \
{                                           \
  push(cpu.reg.val, sizeof(cpu.reg.val)*8); \
}                                           

make_instr_func(push_cs) {
  PUSH_SEGREG(cs)
  return 1;
}

make_instr_func(push_ss) {
  PUSH_SEGREG(ss)
  return 1;
}

make_instr_func(push_ds){
  PUSH_SEGREG(ds)
  return 1;
}

make_instr_func(push_es){
  PUSH_SEGREG(es)
  return 1;
}

make_instr_func(push_fs) {
  PUSH_SEGREG(fs)
  return 2;
}

make_instr_func(push_gs) {
  PUSH_SEGREG(gs)
  return 2;
}

make_instr_func(pusha){
  u32 tmp=cpu.esp;

  push(cpu.eax, 32);
  push(cpu.ecx, 32);
  push(cpu.edx, 32);
  push(cpu.ebx, 32);
  push(tmp, 32);
  push(cpu.ebp, 32);
  push(cpu.esi, 32);
  push(cpu.edi, 32);

  print_asm_0("pusha", "", 1);

  return 1;
}
