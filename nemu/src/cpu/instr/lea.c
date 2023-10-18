#include "cpu-ref/instr_ref.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/modrm.h"
#include "cpu/operand.h"
/*
Put the implementations of `lea' instructions here.
*/

make_instr_func(lea_8d) {
  int len=1;
  OPERAND r,m;
  r.data_size=data_size;
  m.data_size=data_size;
  
  len+=modrm_r_rm(eip+len, &r, &m);

  r.val=m.addr;
  operand_write(&r);

  print_asm_2("lea","",len,&r,&m);
  
  return len;
}
