#ifndef __INSTR_NEG_H__
#define __INSTR_NEG_H__
/*
Put the declarations of `neg' instructions here.
*/

#include "cpu/alu.h"
#include "cpu/instr_helper.h"
#include "cpu/operand.h"
#include "cpu/modrm.h"
#include <stdint.h>

make_instr_func(neg_rm_b);
make_instr_func(neg_rm_v);


#endif
