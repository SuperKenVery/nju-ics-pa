#ifndef __INSTR_PUSH_H__
#define __INSTR_PUSH_H__
/*
Put the declarations of `push' instructions here.
*/

#include <nemu.h>
#include "cpu/instr_helper.h"

void push(u32 data,int size);
make_instr_func(push_ff);
make_instr_func(push_50);
make_instr_func(push_6a);
make_instr_func(push_68);
make_instr_func(push_0e);
make_instr_func(push_16);
make_instr_func(push_1e);
make_instr_func(push_06);
make_instr_func(push_0f);
make_instr_func(push_cs);
make_instr_func(push_ss);
make_instr_func(push_ds);
make_instr_func(push_es);
make_instr_func(push_fs);
make_instr_func(push_gs);

#endif
