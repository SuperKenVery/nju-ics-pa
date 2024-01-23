#ifndef __INSTR_INT_H__
#define __INSTR_INT_H__
/*
Put the declarations of `int' instructions here.

Special note for `int': please use the instruction name `int_' instead of `int'.
*/

#include "cpu-ref/instr_helper_ref.h"
#include "cpu/instr_helper.h"
make_instr_func(int_3);
make_instr_func(into);
make_instr_func(int_imm);

#endif
