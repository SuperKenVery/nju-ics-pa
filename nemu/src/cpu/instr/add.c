#include "cpu-ref/instr_ref.h"
#include "cpu/alu.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/modrm.h"
#include "cpu/operand.h"
#include "cpu/reg.h"
/*
Put the implementations of `add' instructions here.
*/

make_instr_func(add_04) {
  int len=1;
  OPERAND r,imm;
  
  r.type=OPR_REG;
  r.addr=REG_AL;
  r.data_size=8;

  imm.type=OPR_IMM;
  imm.addr=eip+len;
  imm.data_size=8;
  len+=imm.data_size/8;

  operand_read(&r);
  operand_read(&imm);

  r.val=alu_add(r.val,imm.val,8);

  operand_write(&r);

  print_asm_2("add", "", len, &r, &imm);

  return len;
}

make_instr_func(add_05) {
  int len=1;
  OPERAND r,imm;
  
  r.type=OPR_REG;
  r.addr=REG_AX;
  r.data_size=data_size;

  imm.type=OPR_IMM;
  imm.addr=eip+len;
  imm.data_size=data_size;
  len+=imm.data_size/8;;

  operand_read(&r);
  operand_read(&imm);

  r.val=alu_add(r.val,imm.val,data_size);

  operand_write(&r);

  print_asm_2("add", "", len, &r, &imm);

  return len;
}

make_instr_func(add_80) {
  int len=1;
  OPERAND rm,imm;
  rm.data_size=8;
  imm.data_size=8;

  len+=modrm_rm(eip+len,&rm);
  imm.type=OPR_IMM;
  imm.addr=eip+len;
  len+=imm.data_size/8;

  operand_read(&rm);
  operand_read(&imm);

  rm.val=alu_add(rm.val,imm.val,8);

  operand_write(&rm);

  print_asm_2("add", "", len, &imm, &rm);

  return len;
}

make_instr_func(add_81) {
  int len=1;
  OPERAND rm,imm;
  rm.data_size=data_size;
  imm.data_size=data_size;

  len+=modrm_rm(eip+len,&rm);
  imm.type=OPR_IMM;
  imm.addr=eip+len;
  len+=imm.data_size/8;

  operand_read(&rm);
  operand_read(&imm);

  rm.val=alu_add(rm.val,imm.val,data_size);

  operand_write(&rm);

  print_asm_2("add", "", len, &imm, &rm);

  return len;
}

make_instr_func(add_83) {
  int len=1;
  OPERAND rm,imm;
  rm.data_size=data_size;
  imm.data_size=8;

  len+=modrm_rm(eip+len, &rm);
  imm.type=OPR_IMM;
  imm.addr=eip+len;
  len+=imm.data_size/8;

  operand_read(&rm);
  operand_read(&imm);

  rm.val=alu_add(rm.val,sign_ext(imm.val, 8),data_size);

  operand_write(&rm);

  print_asm_2("add", "", len, &imm, &rm);

  return len;
}

make_instr_func(add_00) {
  int len=1;
  OPERAND rm,r;
  rm.data_size=8;
  r.data_size=8;
  len+=modrm_r_rm(eip+len, &r, &rm);

  operand_read(&r);
  operand_read(&rm);

  rm.val=alu_add(rm.val,r.val,8);

  operand_write(&rm);

  print_asm_2("add", "", len, &r, &rm);

  return len;
}

make_instr_func(add_01) {
  int len=1;
  OPERAND rm,r;
  rm.data_size=data_size;
  r.data_size=data_size;
  len+=modrm_r_rm(eip+len, &r, &rm);

  operand_read(&rm);
  operand_read(&r);

  rm.val=alu_add(r.val,rm.val,data_size);

  operand_write(&rm);

  print_asm_2("add", "", len, &rm, &r);
  
  return len;
}

make_instr_func(add_02) {
  int len=1;
  OPERAND rm,r;
  rm.data_size=8;
  r.data_size=8;

  len+=modrm_r_rm(eip+len, &r, &rm);
  operand_read(&rm);
  operand_read(&r);

  r.val=alu_add(rm.val, r.val, 8);

  operand_write(&r);

  print_asm_2("add", "", len, &rm, &r);

  return len;
}

make_instr_func(add_03) {
  int len=1;
  OPERAND rm,r;
  rm.data_size=data_size;
  r.data_size=data_size;

  len+=modrm_r_rm(eip+len, &r, &rm);
  operand_read(&r);
  operand_read(&rm);

  r.val=alu_add(r.val,rm.val,data_size);

  operand_write(&r);

  print_asm_2("add", "", len, &rm, &r);

  return len;
}

