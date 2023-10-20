#include "cpu-ref/instr_ref.h"
#include "cpu/alu.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/modrm.h"
#include "cpu/operand.h"
#include "cpu/reg.h"
/*
Put the implementations of `cmp' instructions here.
*/

make_instr_func(cmp_3c) {
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

  alu_sub(imm.val, al.val, 8);

  print_asm_2("cmp", "", len, &imm, &al);

  return len;
}

make_instr_func(cmp_3d) {
  int len=1;
  OPERAND ax,imm;

  ax.data_size=data_size;
  ax.type=OPR_REG;
  ax.addr=REG_AX;

  imm.data_size=data_size;
  imm.type=OPR_IMM;
  imm.addr=eip+len;
  len+=imm.data_size/8;

  operand_read(&imm);
  operand_read(&ax);

  alu_sub(imm.val,ax.val,data_size);

  print_asm_2("cmp", "", len, &imm, &ax);

  return len;
}

make_instr_func(cmp_80) {
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

  alu_sub(imm.val,rm.val,8);

  print_asm_2("cmp", "", len, &imm, &rm);

  return len;
}

make_instr_func(cmp_81) {
  int len=1;
  OPERAND rm,imm;
  imm.data_size=32;
  rm.data_size=32;
  
  len+=modrm_rm(eip+len,&rm);

  imm.type=OPR_IMM;
  imm.addr=eip+len;
  len+=imm.data_size/8;

  operand_read(&rm);
  operand_read(&imm);

  alu_sub(imm.val,rm.val,32);

  print_asm_2("cmp", "", len, &imm, &rm);

  return len;
}

make_instr_func(cmp_83) {
  int len=1;
  OPERAND rm,imm;
  rm.data_size=data_size;

  len+=modrm_rm(eip+1, &rm);

  imm.type=OPR_IMM;
  imm.data_size=8;
  imm.addr=eip+len;
  len+=imm.data_size/8;

  operand_read(&imm);
  operand_read(&rm);

  alu_sub(sign_ext(imm.val,8), rm.val, data_size);

  print_asm_2("cmp", "", len, &rm, &imm);

  return len;
}

make_instr_func(cmp_38) {
  int len=1;
  OPERAND rm,r;
  rm.data_size=8;
  r.data_size=8;

  len+=modrm_r_rm(eip+len, &r, &rm);
  operand_read(&r);
  operand_read(&rm);

  alu_sub(r.val, rm.val, 8);

  print_asm_2("cmp", "", len, &r, &rm);

  return len;
}

make_instr_func(cmp_39) {
  int len=1;
  OPERAND rm,r;
  rm.data_size=data_size;
  r.data_size=data_size;

  len+=modrm_r_rm(eip+len, &r, &rm);
  operand_read(&r);
  operand_read(&rm);

  alu_sub(r.val,rm.val,data_size);

  print_asm_2("cmp", "", len, &r, &rm);

  return len;
}

make_instr_func(cmp_3a) {
  int len=1;
  OPERAND r,rm;
  r.data_size=8;
  rm.data_size=8;

  len+=modrm_r_rm(eip+len, &r, &rm);
  operand_read(&r);
  operand_read(&rm);

  alu_sub(rm.val,r.val,8);

  print_asm_2("cmp", "", len, &rm, &r);

  return len;
}

make_instr_func(cmp_3b) {
  int len=1;
  OPERAND r,rm;
  r.data_size=data_size;
  rm.data_size=data_size;
  len+=modrm_r_rm(eip+len, &r, &rm);

  operand_read(&r);
  operand_read(&rm);

  alu_sub(rm.val, r.val, data_size);

  print_asm_2("cmp","",len,&rm,&r);

  return len;
}
