#include "cpu-ref/instr_ref.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/operand.h"
#include "cpu/reg.h"
#include "device/port_io.h"
#include "memory/memory.h"
/*
Put the implementations of `out' instructions here.
*/

make_instr_func(out_e6){
  int len=1;
  u8 imm=instr_fetch(eip+len, 1); len++;
  OPERAND al;
  al.type=OPR_REG;
  al.addr=REG_EAX;
  al.data_size=8;
  operand_read(&al);
  pio_write(imm, 1, al.val);
  return len;
}

make_instr_func(out_e7){
  int len=1;
  u8 imm=instr_fetch(eip+len, 1); len++;
  OPERAND al;
  al.type=OPR_REG;
  al.addr=REG_EAX;
  al.data_size=8;
  operand_read(&al);
  pio_write(imm, data_size/8, al.val);
  return len;
}

make_instr_func(out_ee){
  OPERAND dst,content;

  dst.type=OPR_REG;
  dst.addr=REG_EDX;
  dst.data_size=8;
  operand_read(&dst);

  content.type=OPR_REG;
  content.addr=REG_EAX;
  content.data_size=8;
  operand_read(&content);

  pio_write(dst.val, 1, content.val);

  return 1;
}

make_instr_func(out_ef){
  OPERAND dst,content;

  dst.type=OPR_REG;
  dst.addr=REG_EDX;
  dst.data_size=8;
  operand_read(&dst);

  content.type=OPR_REG;
  content.addr=REG_EAX;
  content.data_size=data_size;
  operand_read(&content);

  pio_write(dst.val, data_size/8, content.val);

  return 1;
}
