#include "cpu-ref/instr_ref.h"
#include "cpu/alu.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/modrm.h"
#include "cpu/operand.h"
/*
Put the implementations of `test' instructions here.
*/

make_instr_func(test_a8) {
  int len=1;
  OPERAND al,imm;

  al.data_size=8;
  al.type=OPR_REG;
  al.addr=REG_AL;
  
  imm.data_size=8;
  imm.type=OPR_IMM;
  imm.addr=eip+len;
  len+=imm.data_size/8;

  operand_read(&al);
  operand_read(&imm);

  // alu_add sets more flags than we want
  al.val=al.val&imm.val;
  cpu.eflags.CF=0;
  cpu.eflags.OF=0;

  operand_write(&al);

  print_asm_2("test", "", len, &imm, &al);

  return len;
}

make_instr_func(test_a9) {
  int len=1;
  OPERAND ax,imm;

  ax.data_size=data_size;
  ax.type=OPR_REG;
  ax.addr=REG_AL;
  
  imm.data_size=data_size;
  imm.type=OPR_IMM;
  imm.addr=eip+len;
  len+=imm.data_size/8;

  operand_read(&ax);
  operand_read(&imm);

  // alu_add sets more flags than we want
  ax.val=ax.val&imm.val;
  cpu.eflags.CF=0;
  cpu.eflags.OF=0;

  operand_write(&ax);

  print_asm_2("test", "", len, &imm, &ax);

  return len;
}

make_instr_func(test_f6) {
  int len=1;
  OPERAND rm,imm;
  rm.data_size=8;
  imm.data_size=8;

  len+=modrm_rm(eip+len,&rm);

  imm.type=OPR_IMM;
  imm.addr=eip+len;
  len+=imm.data_size/8;

  operand_read(&imm);
  operand_read(&rm);

  rm.val=rm.val&imm.val;
  cpu.eflags.CF=0;
  cpu.eflags.OF=0;

  operand_write(&rm);

  print_asm_2("test", "", len, &imm, &rm);

  return len;
}

make_instr_func(test_f7) {
  int len=1;
  OPERAND rm,imm;
  rm.data_size=data_size;
  imm.data_size=data_size;

  len+=modrm_rm(eip+len,&rm);

  imm.type=OPR_IMM;
  imm.addr=eip+len;
  len+=imm.data_size/8;

  operand_read(&imm);
  operand_read(&rm);

  rm.val=rm.val&imm.val;
  cpu.eflags.CF=0;
  cpu.eflags.OF=0;

  operand_write(&rm);

  print_asm_2("test", "", len, &imm, &rm);

  return len;
}

make_instr_func(test_84) {
  int len=1;
  OPERAND r,rm;
  r.data_size=8;
  rm.data_size=8;

  len+=modrm_r_rm(eip+len, &r, &rm);
  operand_read(&r);
  operand_read(&rm);

  rm.val=rm.val&r.val;
  cpu.eflags.CF=0;
  cpu.eflags.OF=0;

  operand_write(&rm);

  print_asm_2("test", "", len, &r, &rm);

  return len;
}

make_instr_func(test_85) {
  int len=1;
  OPERAND r,rm;
  r.data_size=data_size;
  rm.data_size=data_size;

  len+=modrm_r_rm(eip+len, &r, &rm);
  operand_read(&r);
  operand_read(&rm);

  rm.val=rm.val&r.val;
  cpu.eflags.CF=0;
  cpu.eflags.OF=0;

  operand_write(&rm);

  print_asm_2("test", "", len, &r, &rm);

  return len;
}
