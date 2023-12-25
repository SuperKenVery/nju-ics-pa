#include "cpu-ref/instr_ref.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/operand.h"
#include "cpu/reg.h"
#include "memory/memory.h"
#include "memory/mmu/segment.h"
#include "nemu.h"
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
    cpu.eip=(cpu.eip+cw.val)&0x0000FFFF;
  }else{
    cpu.eip=cpu.eip+cw.val;
  }

  // Displacement is relative to next instruction
  return len;
}

make_instr_func(call_ff_2) {
  printf("eip=0x%x: call_ff /2\n",cpu.eip);
  int len=1;
  OPERAND rm;
  rm.data_size=data_size;
  len+=modrm_rm(eip+len, &rm);
  operand_read(&rm);

  push(cpu.eip+len, data_size);

  print_asm_1("call","",len,&rm);

  if(data_size==16){
    printf("data size is 16\n");
    cpu.eip=rm.val&0x0000FFFF;
  }else{
    printf("data size is 32, loading 0x%x into eip\n",rm.val);
    cpu.eip=rm.val;
  }

  load_sreg(SREG_CS);

  // Don't modify eip anymore
  return 0;
}

make_instr_func(call_ff_3) {
  printf("call_ff /3\n");
  int len=1;

  push(cpu.cs.val, data_size);
  push(cpu.eip+2+data_size/8, data_size);

  if(cpu.cr0.pe==1 && cpu.eflags.VM==0){
    // Protected mode
    u32 offset=instr_fetch(cpu.eip+len,data_size/8); len+=data_size/8;
    u16 segment=instr_fetch(cpu.eip+len, 2); len+=2;
    cpu.cs.val=segment;
    cpu.eip=offset;
  }else if(cpu.cr0.pe==0){
    // Real mode
    u32 addr=instr_fetch(cpu.eip+len, data_size/8); len+=data_size/8;
    u16 segment=instr_fetch(cpu.eip+len, 2); len+=2;
    cpu.cs.val=segment;
    cpu.eip=addr;
  }else{
    // Virtual 8086 mode
    printf("Call ff /3: Unsupported mode: virtual 8086 mode. \n");
    assert(0);
  }

  load_sreg(SREG_CS);

  // Don't modify eip anymore
  return 0;
}
