#include "cpu-ref/instr_ref.h"
#include "cpu/alu.h"
#include "cpu/instr.h"
#include "cpu/instr_helper.h"
#include "cpu/modrm.h"
#include "cpu/operand.h"
#include "nemu.h"
#include <readline/readline.h>
/*
Put the implementations of `sub' instructions here.
*/


#define make_sub_2x_impl(name,size,reg) \
make_instr_func(name) {  \
  int len=1;             \
  OPERAND r,imm;         \
                         \
  r.data_size=size;      \
  r.type=OPR_REG;        \
  r.addr=reg;            \
                         \
  imm.data_size=size;    \
  imm.type=OPR_IMM;      \
  imm.addr=eip+len;      \
  len+=imm.data_size/8;  \
                         \
  operand_read(&imm);    \
  operand_read(&r);      \
  r.val-=imm.val;        \
  operand_write(&r);     \
                         \
  return len;            \
}

make_sub_2x_impl(sub_2c, 8, REG_AL)
// REG_AX and REG_EAX are the same, which one is used 
// depends on the data size.
make_sub_2x_impl(sub_2d, data_size, REG_AX)

#define make_sub8x_impl(name,rmsize,immsize) \
make_instr_func(name) {                          \
  int len=1;                                     \
  OPERAND rm,imm;                                \
                                                 \
  rm.data_size=rmsize;                           \
  len+=modrm_rm(eip+len, &rm);                   \
                                                 \
  imm.data_size=immsize;                         \
  imm.type=OPR_IMM;                              \
  imm.addr=eip+len;                              \
  len+=imm.data_size/8;                          \
                                                 \
  operand_read(&rm);                             \
  operand_read(&imm);                            \
  rm.val-=sign_ext(imm.val, rmsize);             \
  operand_write(&rm);                            \
                                                 \
  print_asm_2("sub", "", len, &imm, &rm);        \
                                                 \
  return len;                                    \
}

make_sub8x_impl(sub_80, 8, 8)
make_sub8x_impl(sub_81, data_size, data_size)
make_sub8x_impl(sub_83, data_size, 8)

#define make_sub2x_impl(name,rmsize,rsize,rm_on_left) \
make_instr_func(name) {                                  \
int len=1;                                               \
OPERAND rm,r;                                            \
                                                         \
r.data_size=rsize;                                       \
rm.data_size=rmsize;                                     \
len+=modrm_r_rm(eip+len, &r, &rm);                       \
                                                         \
operand_read(&r);                                        \
operand_read(&rm);                                       \
                                                         \
if(rm_on_left){                                          \
  rm.val=alu_sub(rm.val,r.val,rmsize);                   \
  operand_write(&rm);                                    \
  print_asm_2("sub", "", len, &r, &rm);                  \
}else{                                                   \
  r.val=alu_sub(r.val,rm.val,rsize);                     \
  operand_write(&r);                                     \
  print_asm_2("sub", "", len, &rm, &r);                  \
}                                                        \
                                                         \
return len;                                              \
}

make_sub2x_impl(sub_28, 8, 8, true)
make_sub2x_impl(sub_29, data_size, data_size, true)
make_sub2x_impl(sub_2a, 8, 8, false)
make_sub2x_impl(sub_2b, data_size, data_size, false)

