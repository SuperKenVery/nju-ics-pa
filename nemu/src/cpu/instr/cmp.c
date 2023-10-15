#include "cpu-ref/instr_ref.h"
#include "cpu/alu.h"
#include "cpu/instr.h"
#include "cpu/modrm.h"
#include "cpu/operand.h"
/*
Put the implementations of `cmp' instructions here.
*/

make_instr_func(cmp) {
  int len=1;
  OPERAND rm,imm;
  rm.data_size=data_size;
  imm.data_size=data_size;

  len+=modrm_rm(eip+1, &rm);

  imm.type=OPR_IMM;
  imm.data_size=data_size;
  imm.addr=eip+len;
  len++;

  operand_read(&imm);

  alu_sub(rm.val, imm.val, data_size);

  return len;
}
