#include "cpu-ref/instr_ref.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/operand.h"
#include "cpu/reg.h"
#include "device/port_io.h"
#include "memory/memory.h"
/*
Put the implementations of `in' instructions here.
*/

make_instr_func(in_e4){
  u8 imm=instr_fetch(eip+1, 1);
  OPERAND dst;
  dst.type=OPR_REG;
  dst.addr=REG_EAX;
  dst.data_size=8;
  dst.val=pio_read(imm, 1);
  operand_write(&dst);
  return 2;
}

make_instr_func(in_e5){
  u8 imm=instr_fetch(eip+1, 1);
  OPERAND dst;
  dst.type=OPR_REG;
  dst.addr=REG_EAX;
  dst.data_size=data_size;
  dst.val=pio_read(imm, data_size/8);
  operand_write(&dst);
  return 2;
}

make_instr_func(in_ec){
  OPERAND dst,src;
  src.type=OPR_REG;
  src.addr=REG_EDX;
  src.data_size=8;
  operand_read(&src);
  printf("eip=0x%x: IN from port 0x%x\n",eip,src.val);

  dst.type=OPR_REG;
  dst.addr=REG_EAX;
  dst.data_size=data_size;
  dst.val=pio_read(src.val, 1);
  operand_write(&dst);
  return 1;
}

make_instr_func(in_ed){
  OPERAND dst,src;
  src.type=OPR_REG;
  src.addr=REG_EDX;
  src.data_size=8;
  operand_read(&src);

  dst.type=OPR_REG;
  dst.addr=REG_EAX;
  dst.data_size=data_size;
  dst.val=pio_read(src.val, data_size/8);
  operand_write(&dst);
  return 1;
}
