#include "common.h"
#include "memory.h"
#include "string.h"

#include <elf.h>

#ifdef HAS_DEVICE_IDE
#define ELF_OFFSET_IN_DISK 0
#endif

#define STACK_SIZE (1 << 20)

void ide_read(uint8_t *, uint32_t, uint32_t);
void create_video_mapping();
uint32_t get_ucr3();

uint32_t loader()
{
	Elf32_Ehdr *elf;
	Elf32_Phdr *ph, *eph;

#ifdef HAS_DEVICE_IDE
	uint8_t buf[4096];
	ide_read(buf, ELF_OFFSET_IN_DISK, 4096);
	elf = (void *)buf;
	Log("ELF loading from hard disk.");
#else
	elf = (void *)0x0;
	Log("ELF loading from ram disk.");
#endif

	/* Load each program segment */
	// for(int i=0;i<50;i++){
	// 	char *pos=(char*)i;
	// 	Log("Mem addr %d is %x",i,pos);
	// }
	// Log("elf at %p, =%p, elf->e_phoff at %p",&elf,elf,&elf->e_phoff);
	Log("elf at %p",&elf);
	// Log("elf is %p",elf);
	// Log("phoff at %p",&elf->e_phoff);
	ph = (Elf32_Phdr*) ((char*)elf + elf->e_phoff);
	eph = ph + elf->e_phnum;
	for (; ph < eph; ph++)
	{
		if (ph->p_type == PT_LOAD)
		{

			memcpy((char*)ph->p_vaddr, ((char*) elf)+ph->p_offset, ph->p_filesz);

			int zero_out_size=ph->p_memsz-ph->p_filesz;
			if(zero_out_size>0)
				memset(((char*)ph->p_vaddr)+ph->p_filesz, 0, zero_out_size);

#ifdef IA32_PAGE
			/* Record the program break for future use */
			extern uint32_t brk;
			uint32_t new_brk = ph->p_vaddr + ph->p_memsz - 1;
			if (brk < new_brk)
			{
				brk = new_brk;
			}
#endif
		}
	}

	volatile uint32_t entry = elf->e_entry;

#ifdef IA32_PAGE
	mm_malloc(KOFFSET - STACK_SIZE, STACK_SIZE);
#ifdef HAS_DEVICE_VGA
	create_video_mapping();
#endif
	write_cr3(get_ucr3());
#endif
	return entry;
}
