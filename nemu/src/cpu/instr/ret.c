#include "cpu-ref/instr_ref.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/operand.h"
/*
Put the implementations of `ret' instructions here.
*/

make_instr_func(ret_c3) {
  int len=1;
  OPERAND stack,esp;

  // Pop
  esp.type=OPR_REG;
  esp.data_size=32;
  esp.addr=REG_ESP;
  operand_read(&esp);
  
  stack.type=OPR_MEM;
  stack.data_size=data_size;
  stack.addr=esp.val;
  operand_read(&stack);

  esp.val+=stack.data_size/8;
  operand_write(&esp);

  // Write IP
  cpu.eip=stack.val;
  if(stack.data_size==16) cpu.eip=cpu.eip&0xFFFF;

  print_asm_0("ret", "", len);

  // Don't add anything to IP anymore
  return 0;
}
