#ifndef __REG_H__
#define __REG_H__

#include "nemu.h"

typedef struct {
  u32 limit : 16;
  u32 base : 32;
} GDTR;

typedef union {
  struct {
    u32 pe : 1;
    u32 mp : 1;
    u32 em : 1;
    u32 ts : 1;
    u32 et : 1;
    u32 reserve : 26;
    u32 pg : 1;
  };
  u32 val;
} CR0;

typedef struct IDTR {
	u16 size;
	u32 offset;
} IDTR;

typedef union{
	u32 val;
	struct {
		u32 reserved:12;
		u32 pdbr:20; // page directory base register
	};
} CR3;

typedef struct {
  union {
    u16 val;
    struct {
      u32 rpl : 2;
      u32 ti : 1;
      u32 index : 13;
    };
  };

	// Invisible cache
  struct {
    u32 base;
    u32 limit;
    u32 type : 5;
    u32 privilege_level : 2;
    u32 soft_use : 1;
  };
} SegReg;

// define the structure of registers
typedef struct
{
	// general purpose registers
	union
	{
		union
		{
			union
			{
				uint32_t _32;
				uint16_t _16;
				uint8_t _8[2];
			};
			uint32_t val;
		} gpr[8];
		struct
		{ // do not change the order of the registers
			uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
		};
	};

	// EIP
	uint32_t eip;

	// EFLAGS
	union {
		struct
		{
			uint32_t CF : 1;
			uint32_t dummy0 : 1;
			uint32_t PF : 1;
			uint32_t dummy1 : 1;
			uint32_t AF : 1;
			uint32_t dummy2 : 1;
			uint32_t ZF : 1;
			uint32_t SF : 1;
			uint32_t TF : 1;
			uint32_t IF : 1;
			uint32_t DF : 1;
			uint32_t OF : 1;
			uint32_t OLIP : 2;
			uint32_t NT : 1;
			uint32_t dummy3 : 1;
			uint32_t RF : 1;
			uint32_t VM : 1;
			uint32_t dummy4 : 14;
		};
		uint32_t val;
	} eflags;

#ifdef IA32_SEG
	GDTR gdtr;
	// segment registers
	union {
		SegReg segReg[6];
		struct
		{
			SegReg es, cs, ss, ds, fs, gs;
		};
	};
	// control registers, todo: define type CR0
	CR0 cr0;
#else
	uint8_t dummy_seg[142]; // make __ref_ instructions safe to use
#endif
#ifdef IA32_PAGE
	CR3 cr3;
#else
	uint8_t dummy_page[4];
#endif

#ifdef IA32_INTR
	// interrupt
	IDTR idtr; // IDTR, todo: define type IDTR
	uint8_t intr;
#else
	uint8_t dummy_intr[7];
#endif
} CPU_STATE;

enum
{
	REG_AL,
	REG_CL,
	REG_DL,
	REG_BL,
	REG_AH,
	REG_CH,
	REG_DH,
	REG_BH
};
enum
{
	REG_AX,
	REG_CX,
	REG_DX,
	REG_BX,
	REG_SP,
	REG_BP,
	REG_SI,
	REG_DI
};
enum
{
	REG_EAX,
	REG_ECX,
	REG_EDX,
	REG_EBX,
	REG_ESP,
	REG_EBP,
	REG_ESI,
	REG_EDI
};

enum
{
	SREG_ES,
	SREG_CS,
	SREG_SS,
	SREG_DS,
	SREG_FS,
	SREG_GS
};

extern char *reg_names_l[];
extern char *reg_names_w[];
extern char *reg_names_b[];
extern char *sreg_names[];

// print registers
void print_reg();
// get register values
int get_reg_val(const char *reg, bool *success);

#endif
