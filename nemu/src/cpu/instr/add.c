#include "cpu-ref/instr_ref.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/modrm.h"
#include "cpu/operand.h"
/*
Put the implementations of `add' instructions here.
*/

// 00,01,02,03
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
  
  return len;
}
