#ifndef __INSTR_POP_H__
#define __INSTR_POP_H__
/*
Put the declarations of `pop' instructions here.
*/

#include "cpu/instr_helper.h"
make_instr_func(pop_8f);
make_instr_func(pop_58);
make_instr_func(pop_1f);
make_instr_func(pop_07);
make_instr_func(pop_17);
make_instr_func(pop_0f);

#endif
