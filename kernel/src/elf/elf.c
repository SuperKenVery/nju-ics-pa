#include "common.h"
#include "memory.h"
#include "string.h"
#include "x86/cpu.h"

#include <elf.h>
#include <stdint.h>

typedef union{
	uint32_t laddr;
	struct{
		uint32_t offset: 12;
		uint32_t page_entry_index: 10;
		uint32_t page_directory_index: 10;
	};
} laddr_parse_t;

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
	// Log("ELF loading from hard disk.");
#else
	elf = (void *)0x0;
	// Log("ELF loading from ram disk.");
#endif

	/* Load each program segment */
	ph = (void *)elf + elf->e_phoff;
	eph = ph + elf->e_phnum;
	for (; ph < eph; ph++)
	{
		if (ph->p_type == PT_LOAD)
		{
			Log("Found load seg, disk_addr=0x%x, mem_addr=0x%x, filesz=0x%x, memsz=0x%x",ph->p_offset,ph->p_vaddr,ph->p_filesz,ph->p_memsz);
			// laddr_parse_t laddr;
			// laddr.laddr=ph->p_vaddr;
			uint32_t uaddr=mm_malloc(ph->p_vaddr, ph->p_memsz);
			char *dst=(char*)uaddr, *src=((char*)elf)+ph->p_offset;
			for(int offset=0;offset<ph->p_filesz;offset++){
				dst[offset]=src[offset];
			}

			int zero_out_size=ph->p_memsz-ph->p_filesz;
			dst=((char*)uaddr)+ph->p_filesz;
			if(zero_out_size>0) Log("Zeroing out remainings...");
			for(int offset=0;offset<zero_out_size;offset++){
				dst[offset]=0;
			}

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
	// laddr_parse_t entry_p;
	// entry_p.laddr=entry;
	// Log("ELF entry is %x, 0x%x %x %x",entry, entry_p.page_directory_index, entry_p.page_entry_index, entry_p.offset);

#ifdef IA32_PAGE
	mm_malloc(KOFFSET - STACK_SIZE, STACK_SIZE);
#ifdef HAS_DEVICE_VGA
	create_video_mapping();
#endif
	// CR3 ucr;
	// ucr.val=get_ucr3();
	// Log("page directory base addr: 0x%x",ucr.page_directory_base<<12);
	write_cr3(get_ucr3());
#endif
	return entry;
}
