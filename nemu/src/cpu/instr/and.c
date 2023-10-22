#include "cpu-ref/instr_ref.h"
#include "cpu/alu.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/modrm.h"
#include "cpu/operand.h"
#include "cpu/reg.h"
/*
Put the implementations of `and' instructions here.
*/

void do_and(OPERAND *dest, OPERAND *src, int len){
  operand_read(dest);
  operand_read(src);

  if(src->data_size<dest->data_size){
    src->val=sign_ext(src->val, src->data_size);
  }
  
  dest->val=alu_and(src->val, dest->val, dest->data_size);

  operand_write(dest);

  print_asm_2("and", "", len, src, dest);
}

make_instr_func(and_24) {
  int len=1;
  OPERAND al,imm;

  al.data_size=8;
  al.type=OPR_REG;
  al.addr=REG_AL;

  imm.data_size=8;
  imm.type=OPR_IMM;
  imm.addr=eip+len;
  len+=imm.data_size/8;

  do_and(&al, &imm, len);

  return len;
}

make_instr_func(and_25) {
  int len=1;
  OPERAND ax,imm;

  ax.type=OPR_REG;
  ax.data_size=data_size;
  ax.addr=REG_AX;

  imm.type=OPR_IMM;
  imm.data_size=data_size;
  imm.addr=eip+len;
  len+=imm.data_size/8;

  do_and(&ax,&imm,len);

  return len;
}

make_instr_func(and_80) {
  int len=1;
  OPERAND rm,imm;

  rm.data_size=8;
  len+=modrm_rm(eip+len,&rm);
  
  imm.data_size=8;
  imm.type=OPR_IMM;
  imm.addr=eip+len;
  len+=imm.data_size/8;

  do_and(&rm,&imm,len);

  return len;
}

make_instr_func(and_81) {
  int len=1;
  OPERAND rm,imm;

  rm.data_size=data_size;
  len+=modrm_rm(eip+len,&rm);
  
  imm.data_size=data_size;
  imm.type=OPR_IMM;
  imm.addr=eip+len;
  len+=imm.data_size/8;

  do_and(&rm,&imm,len);

  return len;
}

make_instr_func(and_83) {
  int len=1;
  OPERAND rm,imm;

  rm.data_size=data_size;
  len+=modrm_rm(eip+len,&rm);
  
  imm.data_size=8;
  imm.type=OPR_IMM;
  imm.addr=eip+len;
  len+=imm.data_size/8;

  do_and(&rm,&imm,len);

  return len;
}

make_instr_func(and_20) {
  int len=1;
  OPERAND rm,r;
  rm.data_size=8;
  r.data_size=8;

  len+=modrm_r_rm(eip+len, &r, &rm);

  do_and(&rm, &r, len);

  return len;
}

make_instr_func(and_21) {
  int len=1;
  OPERAND rm,r;
  rm.data_size=data_size;
  r.data_size=data_size;

  len+=modrm_r_rm(eip+len, &r, &rm);

  do_and(&rm, &r, len);

  return len;
}

make_instr_func(and_22) {
  int len=1;
  OPERAND r,rm;
  rm.data_size=8;
  r.data_size=8;

  len+=modrm_r_rm(eip+len, &r, &rm);

  do_and(&r, &rm, len);

  return len;
}

make_instr_func(and_23) {
  int len=1;
  OPERAND r,rm;
  rm.data_size=data_size;
  r.data_size=data_size;

  len+=modrm_r_rm(eip+len, &r, &rm);

  do_and(&r, &rm, len);

  return len;
}
