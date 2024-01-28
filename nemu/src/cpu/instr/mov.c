#include "cpu-ref/instr_helper_ref.h"
#include "cpu-ref/instr_ref.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/modrm.h"
#include "cpu/operand.h"
#include "cpu/reg.h"
#include "debug.h"
#include "memory/memory.h"
#include "memory/mmu/segment.h"
#include "nemu.h"
#include <SDL/SDL_version.h>

static void instr_execute_2op() 
{
	operand_read(&opr_src);
	opr_dest.val = opr_src.val;
	operand_write(&opr_dest);
}

make_instr_impl_2op(mov, r, rm, b)
make_instr_impl_2op(mov, r, rm, v)
make_instr_impl_2op(mov, rm, r, b)
make_instr_impl_2op(_mov, rm, r, v)
make_instr_impl_2op(mov, i, rm, b)
make_instr_impl_2op(mov, i, rm, v)
make_instr_impl_2op(mov, i, r, b)
make_instr_impl_2op(mov, i, r, v)
make_instr_impl_2op(mov, a, o, b)
make_instr_impl_2op(mov, a, o, v)
make_instr_impl_2op(mov, o, a, b)
make_instr_impl_2op(mov, o, a, v)

make_instr_func(mov_rm2r_v){
  if(eip>=0xc0031af7 && eip<=0xc0031b9a){
    printf("[%x: eax=0x%x ",cpu.eax);
  }
  int result=_mov_rm2r_v(eip, opcode);
  if(eip>=0xc0031af7 && eip<=0xc0031b9a) printf("R *0x%x] ",eip&0xfffff, opr_src.addr);
  return result;
}

make_instr_func(mov_zrm82r_v) {
	int len = 1;
	OPERAND r, rm;
	r.data_size = data_size;
	rm.data_size = 8;
	len += modrm_r_rm(eip + 1, &r, &rm);
	
	operand_read(&rm);
	r.val = rm.val;
	operand_write(&r);

	print_asm_2("mov", "", len, &rm, &r);
	return len;
}

make_instr_func(mov_zrm162r_l) {
        int len = 1;
        OPERAND r, rm;
        r.data_size = 32;
        rm.data_size = 16;
        len += modrm_r_rm(eip + 1, &r, &rm);

        operand_read(&rm);
        r.val = rm.val;
        operand_write(&r);
	print_asm_2("mov", "", len, &rm, &r);
        return len;
}

make_instr_func(mov_srm82r_v) {
        int len = 1;
        OPERAND r, rm;
        r.data_size = data_size;
        rm.data_size = 8;
        len += modrm_r_rm(eip + 1, &r, &rm);
        
	operand_read(&rm);
        r.val = sign_ext(rm.val, 8);
        operand_write(&r);
	print_asm_2("mov", "", len, &rm, &r);
        return len;
}

make_instr_func(mov_srm162r_l) {
        int len = 1;
        OPERAND r, rm;
        r.data_size = 32;
        rm.data_size = 16;
        len += modrm_r_rm(eip + 1, &r, &rm);
        operand_read(&rm);
        r.val = sign_ext(rm.val, 16);
        operand_write(&r);

	print_asm_2("mov", "", len, &rm, &r);
        return len;
}

make_instr_func(mov_8e) {
  int len=1;
  OPERAND sreg, rm;
  sreg.data_size=16;
  rm.data_size=16;
  len+=modrm_r_rm(eip+len, &sreg, &rm);
  sreg.type=OPR_SREG;

  operand_read(&rm);
  sreg.val=rm.val;
  operand_write(&sreg);

  load_sreg(sreg.addr);
  print_asm_2("mov", "", len, &sreg, &rm);

  return len;
}

make_instr_func(mov_8c) {
  int len=1;
  OPERAND sreg,rm;
  sreg.data_size=16;
  rm.data_size=16;
  len+=modrm_r_rm(eip+len, &sreg, &rm);
  sreg.type=OPR_SREG;

  operand_read(&sreg);
  rm.val=sreg.val;
  operand_write(&rm);

  print_asm_2("mov", "", len, &rm, &sreg);

  return len;
}

make_instr_func(mov_from_cr) {
  int len=1;
  OPERAND cr,rm;
  cr.data_size=32;
  rm.data_size=32;
  len+=modrm_r_rm(eip+len, &cr, &rm);
  cr.type=OPR_CREG;

  operand_read(&cr);
  rm.val=cr.val;
  operand_write(&rm);

  print_asm_2("mov", "", len, &cr, &rm);

  return len;
}

make_instr_func(mov_to_cr) {
  int len=1;
  OPERAND cr,rm;
  cr.data_size=32;
  rm.data_size=32;
  len+=modrm_r_rm(eip+len, &cr, &rm);
  cr.type=OPR_CREG;

  operand_read(&rm);
  cr.val=rm.val;
  operand_write(&cr);

  // printf("Moving TO CR addr=%d\n", cr.addr);
  // CR3 c;
  // c.val=cr.val;
  // hexdump_pointer((hw_mem+(c.pdbr<<12)), 1024*4);

  print_asm_2("mov", "", len, &rm, &cr);

  return len;
}
