#include "cpu-ref/instr_ref.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/modrm.h"
#include "cpu/operand.h"
/*
Put the implementations of `xor' instructions here.
*/

make_instr_func(xor_31) {
  int len=1;
  OPERAND rm,r;
  rm.data_size=data_size;
  r.data_size=data_size;

  len+=modrm_r_rm(eip+len, &r, &rm);

  operand_read(&r);
  operand_read(&rm);

  rm.val=alu_xor(rm.val,r.val,data_size);

  operand_write(&rm);
  return len;
}
