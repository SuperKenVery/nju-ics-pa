#include "cpu-ref/instr_ref.h"
#include "cpu/alu.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/modrm.h"
#include "cpu/operand.h"
/*
Put the implementations of `cmp' instructions here.
*/

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

  alu_sub(rm.val, sign_ext(imm.val,8), data_size);

  print_asm_2("cmp", "", len, &rm, &imm);
  printf("cmp83: len=%d, rm=%d, rm.addr=%d, imm=%d\n",len,rm.val,rm.addr,imm.val);

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

  alu_sub(r.val, rm.val, data_size);

  print_asm_2("cmp","",len,&rm,&r);

  return len;
}
