#include "cpu-ref/instr_ref.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/modrm.h"
#include "cpu/operand.h"
/*
Put the implementations of `inc' instructions here.
*/

make_instr_func(inc_fe) {
  int len=1;
  OPERAND rm;
  rm.data_size=8;

  len+=modrm_rm(eip+len,&rm);

  operand_read(&rm);
  rm.val++;
  operand_write(&rm);

  return len;
}

make_instr_func(inc_ff) {
  int len=1;
  OPERAND rm;
  rm.data_size=data_size;

  len+=modrm_rm(eip+len, &rm);

  operand_read(&rm);
  rm.val++;
  operand_write(&rm);

  return len;
}

make_instr_func(inc_40) {
  int len=1;
  OPERAND r;
  r.data_size=data_size;

  r.type=OPR_REG;
  r.addr=opcode%8;

  operand_read(&r);
  r.val++;
  operand_write(&r);

  return len;
}
