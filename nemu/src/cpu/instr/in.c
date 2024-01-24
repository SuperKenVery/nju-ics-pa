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
  int len=1;
  OPERAND dst,imm;
  imm.type=OPR_IMM;
  imm.data_size=8;
  imm.addr=eip+len; len+=imm.data_size/8;
  operand_read(&imm);
  dst.type=OPR_REG;
  dst.addr=REG_EAX;
  dst.data_size=8;
  dst.val=pio_read(imm.val, 1);
  operand_write(&dst);

  print_asm_2("in", "", len, &imm, &dst);
  
  return 2;
}

make_instr_func(in_e5){
  int len=1;
  OPERAND dst,imm;
  imm.type=OPR_IMM;
  imm.data_size=8;
  imm.addr=eip+len; len+=imm.data_size/8;
  operand_read(&imm);
  dst.type=OPR_REG;
  dst.addr=REG_EAX;
  dst.data_size=data_size;
  dst.val=pio_read(imm.val, data_size/8);
  operand_write(&dst);

  print_asm_2("in", "", len, &imm, &dst);
  return len;
}

make_instr_func(in_ec){
  int len=1;
  OPERAND dst,src;
  src.type=OPR_REG;
  src.addr=REG_EDX;
  src.data_size=16;
  operand_read(&src);
  printf("eip=0x%x: IN from port 0x%x\n",eip,src.val);

  dst.type=OPR_REG;
  dst.addr=REG_EAX;
  dst.data_size=data_size;
  dst.val=pio_read(src.val, 1);
  operand_write(&dst);

  print_asm_2("in", "", len, &src, &dst);
  
  return len;
}

make_instr_func(in_ed){
  int len=1;
  OPERAND dst,src;
  src.type=OPR_REG;
  src.addr=REG_EDX;
  src.data_size=16;
  operand_read(&src);

  dst.type=OPR_REG;
  dst.addr=REG_EAX;
  dst.data_size=data_size;
  dst.val=pio_read(src.val, data_size/8);
  operand_write(&dst);

  print_asm_2("in", "", len, &src, &dst);
  
  return len;
}
