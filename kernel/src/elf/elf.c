#include "common.h"
#include "memory.h"
#include "string.h"

#include <elf.h>
#include <string.h>

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
	Log("About to load ELF, elf is %p",elf);
	ph = (void *)elf + elf->e_phoff;
	Log("AAA");
	eph = ph + elf->e_phnum;
	Log("C");
	for (; ph < eph; ph++)
	{
		if (ph->p_type == PT_LOAD)
		{
			Log("Found a load segment. Loading file...");

			memcpy((char*)ph->p_vaddr, ((char*) elf)+ph->p_offset, ph->p_filesz);

			int zero_out_size=ph->p_memsz-ph->p_filesz;
			if(zero_out_size>0) Log("About to zero out...");
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
