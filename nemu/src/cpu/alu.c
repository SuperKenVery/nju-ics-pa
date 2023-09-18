#include "cpu/alu.h"
#include "cpu-ref/instr_ref.h"
#include "cpu/cpu.h"

void hexdump(char* name, u64 x){
	printf("%s: %llu (%p %p)\n",name, x,(void*)((u32)(x>>32)),(void*)((u32)x));
}

bool positive(i32 x, usize size){
	u8 sign_bit=x>>(size-1);
	return !sign_bit;
}

// u32 cut(u32 x, usize size){
// 	assert(size<=32);
// 	// Shift bits is mod by 31
// 	// So take 32bit as special case
// 	if(size==32) return x;
	
// 	u32 mask=(1<<size)-1;
// 	return x&mask;	
// }

u64 cut(u64 x, usize size){
	if(size==64) return x;

	u64 mask=(((u64)1)<<size)-1;
	return x&mask;
}

u64 cutdbg(u64 x, usize size){
	printf("===cutdbg\n");
	hexdump("x",x);
	hexdump("size",size);
	if(size==64) return x;

	u64 mask=(((u64)1)<<size)-1;
	hexdump("mask", mask);
	printf("===cutdbg done\n");
	return x&mask;
}

void set_OF_add(i32 a, i32 b, usize size){
	a=cut(a,size);
	b=cut(b,size);
	i32 r=cut(a+b,size);
	
	if(
		(positive(a,size)==positive(b, size)) &&
		(positive(r, size)!=positive(a,size))
	){
		cpu.eflags.OF=1;
	}else{
		cpu.eflags.OF=0;
	}
}

void set_OF_sub(i32 a, i32 b, i32 result, usize size){
	a=cut(a,size);
	b=cut(b,size);

	if(
		(positive(a,size)!=positive(b,size)) &&
		(positive(a,size)!=positive(result,size))
	){
		cpu.eflags.OF=1;
	}else{
		cpu.eflags.OF=0;
	}
}

void set_SF(i32 result, usize size){
	if(positive(result, size)){
		cpu.eflags.SF=0;
	}else{
		cpu.eflags.SF=1;
	}
}

void set_ZF(i32 result, usize size){
	result=cut(result,size);
	if(result==0){
		cpu.eflags.ZF=1;
	}else{
		cpu.eflags.ZF=0;
	}
}

// AF is not required for the pa
// void set_AF()

void set_CF_add(u32 a, u32 b, usize size){
	a=cut(a,size);
	b=cut(b,size);
	u32 r=cut(a+b,size);
	if(r<a){
		cpu.eflags.CF=1;
	}else{
		cpu.eflags.CF=0;
	}
}

void set_CF_sub(u32 a, u32 b, usize size){
	if(a<b){
		cpu.eflags.CF=1;
	}else{
		cpu.eflags.CF=0;
	}
}

void set_PF(u32 x){
	u8 count=0;
	for(int i=0;i<8;i++){
		u8 bit=x&1;
		if(bit) count++;
		x=x>>1;
	}

	if(count%2==1){
		// Odd
		cpu.eflags.PF=0;
	}else{
		// Even
		cpu.eflags.PF=1;
	}
}


// ====== ALU Functions ======

// Add: DEST <- DEST + SRC
// Flags: OF, SF, ZF, AF, CF, PF
uint32_t alu_add(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_add(src, dest, data_size);
#else
	u32 result=src+dest;
	result=cut(result,data_size);
	set_OF_add(src,dest,data_size);
	set_SF(result,data_size);
	set_ZF(result,data_size);
	set_CF_add(src,dest,data_size);
	set_PF(result);
	
	return result;
#endif
}

uint32_t alu_adc(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_adc(src, dest, data_size);
#else
	src=cut(src,data_size);
	dest=cut(dest,data_size);
	u32 a=alu_add(src,cpu.eflags.CF,data_size);
	u32 CF_backup=cpu.eflags.CF;
	u32 b=alu_add(a,dest,data_size);

	cpu.eflags.CF|=CF_backup;

	if(
		(positive(src,data_size)==positive(dest, data_size)) &&
		positive(src, data_size)!=positive(b, data_size)
	){
		cpu.eflags.OF=1;
	}else{
		cpu.eflags.OF=0;
	}
	return b;
#endif
}

// DEST <- DEST - SRC
// OF, SF, ZF, AF, PF, CF
uint32_t alu_sub(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sub(src, dest, data_size);
#else
	src=cut(src,data_size);
	dest=cut(dest,data_size);
	u32 result=alu_add(dest,-src,data_size);


	set_OF_sub(dest, src, result, data_size);
	set_SF(result, data_size);
	set_ZF(result, data_size);
	set_PF(result);
	set_CF_sub(dest, src, data_size);
	
	return result;
#endif
}

uint32_t alu_sbb(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sbb(src, dest, data_size);
#else
	src=cut(src,data_size);
	dest=cut(dest,data_size);
	u8 CF_backup=cpu.eflags.CF;
	u32 a=alu_sub(src,dest,data_size);
	u8 stage1_cf=cpu.eflags.CF;
	u32 result=alu_sub(CF_backup,a,data_size);

	set_OF_sub(dest, src, result, data_size);
	set_SF(result, data_size);
	set_ZF(result, data_size);
	set_PF(result);
	cpu.eflags.CF|=stage1_cf;
	
	return result;
#endif
}

#define MUL_SET_FLAGS(reg) \
	{ 									 \
		if(reg==0){ 			 \
			cpu.eflags.CF=0; \
			cpu.eflags.OF=0; \
		}else {						 \
			cpu.eflags.CF=1; \
			cpu.eflags.OF=1; \
		}									 \
	}
uint64_t alu_mul(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mul(src, dest, data_size);
#else
	src=cut(src,data_size);
	dest=cut(dest,data_size);
	u64 result;
	if(data_size!=32)
		result=cut(src*dest,data_size*2);
	else
		result=((u64)src)*dest;
	u8 ah; u16 dx; u32 edx;

	switch(data_size){
		case 8:
			ah=cut(result>>8,8);
			MUL_SET_FLAGS(ah)
			break;
		case 16:
			dx=cut(result>>16,16);
			MUL_SET_FLAGS(dx)
			break;
		case 32:
			edx=cut(result>>32,32);
			MUL_SET_FLAGS(edx)
			break;
		default:
			// Let the assert fail
			// It will print file and line number
			assert(data_size==8||data_size==16||data_size==32);
	}

	// SF, ZF, AF, PF, CF undefined
	
	return result;
#endif
}

inline void set_CFOF(){
	cpu.eflags.CF=1;
	cpu.eflags.OF=1;
}
inline void clear_CFOF(){
	cpu.eflags.CF=0;
	cpu.eflags.OF=0;
}
int64_t alu_imul(int32_t src, int32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imul(src, dest, data_size);
#else
	src=sign_ext(cut(src,data_size),data_size);
	dest=sign_ext(cut(dest,data_size),data_size);
	i64 result=((i64)src)*dest;
	i8 r8; i16 r16; i32 r32; i64 r64;
	switch(data_size){
		case 8:
			r8=cut(result,8);
			r16=cut(result,16);
			if(r16==(i16)r8) clear_CFOF();
			else set_CFOF();
			break;
		case 16:
			r16=cut(result,16);
			r32=cut(result,32);
			if(r32==(i32)r16) clear_CFOF();
			else set_CFOF();
			break;
		case 32:
			r32=cut(result,32);
			r64=result;
			if(r64==(i64)r32) clear_CFOF();
			else set_CFOF();
			break;
		default:
			assert(data_size==8||data_size==16||data_size==32);
	}

	// SF, ZF, AF, PF undefined
	
	return result;
#endif
}

// need to implement alu_mod before testing
uint32_t alu_div(uint64_t src, uint64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_div(src, dest, data_size);
#else
	src=cut(src,data_size);
	dest=cut(dest,data_size);
	assert(src!=0);
	
	u32 res=dest/src;
	res=cut(res,data_size);
	return res;
#endif
}

// need to implement alu_imod before testing
int32_t alu_idiv(int64_t src, int64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_idiv(src, dest, data_size);
#else
	src=sign_ext(cut(src,data_size),data_size);
	dest=sign_ext(cut(dest,data_size),data_size);
	assert(src!=0);
	
	i32 res=dest/src;
	return res;
#endif
}

uint32_t alu_mod(uint64_t src, uint64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mod(src, dest);
#else
	src=cut(src,data_size);
	dest=cut(dest,data_size);
	assert(src!=0);

	u32 res=dest%src;
	res=cut(res,data_size);
	return res;
#endif
}

int32_t alu_imod(int64_t src, int64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imod(src, dest);
#else
	src=sign_ext(cut(src,data_size),data_size);
	dest=sign_ext(cut(dest,data_size),data_size);
	assert(src!=0);

	i32 res=dest%src;
	res=cut(res,data_size);
	return res;
#endif
}

uint32_t alu_and(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_and(src, dest, data_size);
#else
	src=cut(src,data_size);
	dest=cut(dest,data_size);
	cpu.eflags.CF=0;
	cpu.eflags.OF=0;
	u32 result=src&dest;
	set_PF(result);
	set_SF(result, data_size);
	set_ZF(result, data_size);
	return result;
#endif
}

uint32_t alu_xor(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_xor(src, dest, data_size);
#else
	src=cut(src,data_size);
	dest=cut(dest,data_size);
	u32 result=src^dest;

	cpu.eflags.CF=0;
	cpu.eflags.OF=0;
	set_SF(result, data_size);
	set_ZF(result, data_size);
	set_PF(result);
	
	return result;
#endif
}

uint32_t alu_or(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_or(src, dest, data_size);
#else
	src=cut(src,data_size);
	dest=cut(dest,data_size);
	u32 result=src|dest;
	cpu.eflags.OF=0;
	cpu.eflags.CF=0;
	set_SF(result, data_size);
	set_ZF(result, data_size);
	set_PF(result);
	
	return result;
#endif
}

uint32_t alu_shl(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shl(src, dest, data_size);
#else
	src=cut(src,data_size);
	dest=cut(dest,data_size);
	u32 count=src;

	while(count!=0){
		cpu.eflags.CF=(dest>>(data_size-1))&1;
		dest=dest*2;
		dest=cut(dest,data_size);
		count=count-1;
	}

	if(src==1){
		cpu.eflags.OF=(
			((dest>>(data_size-1)) &1 ) != cpu.eflags.CF
		);
	} // else OF is undefined

	set_ZF(dest, data_size);
	set_PF(dest);
	set_SF(dest, data_size);
	return dest;
#endif
}

uint32_t alu_shr(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shr(src, dest, data_size);
#else
	src=cut(src,data_size);
	dest=cut(dest,data_size);
	u32 count=src;
	while(count!=0){
		cpu.eflags.CF=dest&1;
		dest=dest/2; // Unsigned divide
		dest=cut(dest,data_size);
		count=count-1;
	}

	if(src==1){
		cpu.eflags.OF=(dest>>(data_size-1))&1;
	} // else OF is undefined

	set_ZF(dest, data_size);
	set_PF(dest);
	set_SF(dest, data_size);
	
	return dest;
#endif
}

uint32_t alu_sar(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sar(src, dest, data_size);
#else
	src=cut(src,data_size);
	dest=cut(dest,data_size);
	u32 count=src;
	i32 result=sign_ext(dest, data_size);
	while(count!=0){
		cpu.eflags.CF=result&1;
		result=result>>1;
		count=count-1;
	}
	result=cut(result,data_size);

	if(src==1){
		cpu.eflags.OF=0;
	} // else OF is undefined
	set_ZF(result, data_size);
	set_PF(result);
	set_SF(result, data_size);
	
	return result;
#endif
}

uint32_t alu_sal(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sal(src, dest, data_size);
#else
	return alu_shl(src,dest,data_size);
#endif
}
