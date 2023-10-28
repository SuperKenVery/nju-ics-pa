#ifndef __INSTR_JMP_H__
#define __INSTR_JMP_H__

#include "cpu/instr_helper.h"
make_instr_func(jmp_near);
make_instr_func(jmp_eb);
make_instr_func(jmp_near_indirect);

#endif
