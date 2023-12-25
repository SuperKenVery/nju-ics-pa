#include "cpu-ref/instr_ref.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/operand.h"
#include "cpu/reg.h"
/*
Put the implementations of `call' instructions here.
*/

make_instr_func(call_e8) {
  int len=1;
  OPERAND cw;
  cw.type=OPR_IMM;
  cw.data_size=data_size;
  cw.addr=eip+len;
  len+=cw.data_size/8;
  operand_read(&cw);

  push(cpu.eip+len,data_size);

  print_asm_1("call","",len,&cw);

  if(data_size==16){
    cpu.eip=(cpu.eip+cw.val)&0xFFFF;
  }else{
    cpu.eip=cpu.eip+cw.val;
  }

  // Displacement is relative to next instruction
  return len;
}

make_instr_func(call_ff_2) {
  int len=1;
  OPERAND rm;
  rm.data_size=data_size;
  len+=modrm_rm(eip+len, &rm);
  operand_read(&rm);

  push(cpu.eip, data_size);
  if(data_size==16){
    cpu.eip=rm.val&0x0000FFFF;
  }else{
    cpu.eip=rm.val;
  }

  // Don't modify ip anymore
  return 0;
}

make_instr_func(call_ff_3) {
  int len=1;
  push(cpu.cs,16);
  push(cpu.eip, data_size);

  u16 selector=instr_fetch(eip+len, 2);
  len+=2;
  u32 offset=0;
  if(data_size==16){
    offset=instr_fetch(eip+len, 2);
    offset=offset&0x0000FFFF;
    len+=2;
  }else{
    offset=instr_fetch(eip+len,4);
    len+=4;
  }
  cpu.eip=offset;
}
