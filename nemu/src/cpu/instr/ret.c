#include "cpu-ref/instr_ref.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/operand.h"
#include "cpu/reg.h"
/*
Put the implementations of `ret' instructions here.
*/

make_instr_func(ret_c3) {
  int len=1;

  // Write IP
  cpu.eip=pop(data_size);
  if(data_size==16) cpu.eip=cpu.eip&0xFFFF;

  print_asm_0("ret", "", len);

  // Don't add anything to IP anymore
  return 0;
}

make_instr_func(ret_c2) {
  int len=1;
  OPERAND imm;

  imm.type=OPR_IMM;
  imm.data_size=16;
  imm.addr=cpu.eip+len;
  len+=imm.data_size/8;
  operand_read(&imm);

  cpu.eip=pop(data_size);
  if(data_size==16) cpu.eip=cpu.eip&0xFFFF;

  // Add imm to esp
  OPERAND esp;
  esp.type=OPR_REG;
  esp.addr=REG_ESP;
  esp.data_size=32;
  operand_read(&esp);
  esp.val=esp.val+imm.val;
  operand_read(&esp);

  print_asm_1("ret", "(eip is wrong)", len, &imm);

  // Don't add anything to IP anymore
  return 0;
}
