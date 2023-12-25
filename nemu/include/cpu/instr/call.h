#ifndef __INSTR_CALL_H__
#define __INSTR_CALL_H__
#include <cpu/instr_helper.h>
/*
Put the declarations of `call' instructions here.
*/

make_instr_func(call_e8);
make_instr_func(call_ff_2);
make_instr_func(call_ff_3);

#endif
