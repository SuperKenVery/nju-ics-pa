#ifndef __INSTR_OUT_H__
#define __INSTR_OUT_H__
/*
Put the declarations of `out' instructions here.
*/

#include "cpu/instr_helper.h"
make_instr_func(out_e6);
make_instr_func(out_e7);
make_instr_func(out_ee);
make_instr_func(out_ef);
#endif
