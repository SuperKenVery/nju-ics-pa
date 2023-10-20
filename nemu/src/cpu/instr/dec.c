#include "cpu-ref/instr_ref.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/modrm.h"
#include "cpu/operand.h"
/*
Put the implementations of `dec' instructions here.
*/

make_instr_func(dec_fe) {
  int len=1;
  OPERAND rm;
  rm.data_size=8;

  len+=modrm_rm(eip+len, &rm);

  operand_read(&rm);
  rm.val=alu_sub(1,rm.val,8);
  operand_write(&rm);

  print_asm_1("dec", "", len, &rm);

  return len;
}

make_instr_func(dec_ff) {
  int len=1;
  OPERAND rm;
  rm.data_size=data_size;

  len+=modrm_rm(eip+len, &rm);

  operand_read(&rm);
  rm.val=alu_sub(1,rm.val,data_size);
  operand_write(&rm);

  print_asm_1("dec", "", len, &rm);

  return len;
}

make_instr_func(dec_48) {
  int len=1;
  OPERAND r;
  r.type=OPR_REG;
  r.data_size=data_size;
  r.addr=opcode%8;

  operand_read(&r);
  r.val=alu_sub(1,r.val,data_size);
  operand_write(&r);

  return len;
}
