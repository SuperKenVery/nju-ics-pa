#include "common.h"
#include "debug.h"
#include "string.h"
#include "sys/types.h"
#include <ctype.h>

typedef struct
{
	char *name;
	uint32_t size;
	uint32_t disk_offset;
} file_info;

typedef struct
{
	bool used;
	uint32_t index;
	uint32_t offset;
} Fstate;

enum
{
	SEEK_SET,
	SEEK_CUR,
	SEEK_END
};

static const file_info file_table[] = {
	{"1.rpg", 188864, 1048576}, {"2.rpg", 188864, 1237440}, {"3.rpg", 188864, 1426304}, {"4.rpg", 188864, 1615168}, {"5.rpg", 188864, 1804032}, {"abc.mkf", 1022564, 1992896}, {"ball.mkf", 134704, 3015460}, {"data.mkf", 66418, 3150164}, {"desc.dat", 16027, 3216582}, {"fbp.mkf", 1128064, 3232609}, {"fire.mkf", 834728, 4360673}, {"f.mkf", 186966, 5195401}, {"gop.mkf", 11530322, 5382367}, {"map.mkf", 1496578, 16912689}, {"mgo.mkf", 1577442, 18409267}, {"m.msg", 188232, 19986709}, {"mus.mkf", 331284, 20174941}, {"pat.mkf", 8488, 20506225}, {"rgm.mkf", 453202, 20514713}, {"rng.mkf", 4546074, 20967915}, {"sss.mkf", 557004, 25513989}, {"voc.mkf", 1997044, 26070993}, {"wor16.asc", 5374, 28068037}, {"wor16.fon", 82306, 28073411}, {"word.dat", 5650, 28155717}, {"credits_bgm.wav", 6631340, 1048576},
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

Fstate files[NR_FILES + 3];

void ide_read(uint8_t *, uint32_t, uint32_t);
void ide_write(uint8_t *, uint32_t, uint32_t);

/* Please implement a simple file system here */

int fs_open(const char *pathname, int flags)
{
	size_t len=strlen(pathname);
	char low_pathname[100];
	for(int i=0;i<len;i++){
		low_pathname[i]=tolower(((unsigned char*)pathname)[i]);
	}
	low_pathname[len]=0;
	Log("Lowered name: %s",low_pathname);
	for(int findex=0;findex<NR_FILES;findex++){
		const file_info *f=&file_table[findex];
		if(strcmp(f->name,low_pathname)==0){
			for(int fd=3;fd<NR_FILES+3;fd++)
				if(files[fd].used==false){
					files[fd].used=true;
					files[fd].index=findex;
					files[fd].offset=0;
					// Log("Opened %s:",pathname);
					// char buf[64];
					// int headsize=f->size>64?64:f->size;
					// ide_read((void*)buf,f->disk_offset, headsize);
					// hexdump_pointer(buf, headsize);
					return fd;
				}

			Log("File found, but no remaining fd");
			return -1;
		}
	}
	Log("File not found: %s",low_pathname);
	assert(0);
	return -1;
}

size_t fs_read(int fd, void *buf, size_t len)
{
	assert(fd > 2);
	Fstate *f=&files[fd];
	if(f->used==false){
		Log("Reading from a not opened fd");
		return -1;
	}
	if(f->offset+len>file_table[f->index].size){
		Log("Reading more than file size, clipping");
		len=file_table[f->index].size-f->offset;
	}
	if(len<=0){
		Log("len<0");
		return 0;
	}
	ide_read(buf, file_table[f->index].disk_offset+f->offset, len);
	// Log("fs_read file=%s",file_table[f->index].name);
	// hexdump_pointer(buf, len);
	// if(strcmp(file_table[f->index].name,"data.mkf")==0){
	// 	Log("Reading data.mkf, offset=%d, len=%d",f->offset,len);
	// 	hexdump_pointer(buf, len);
	// }
	f->offset+=len;
	return len;
}

size_t fs_write(int fd, void *buf, size_t len)
{
	// assert(fd <= 2);
	if(fd<=2){
#ifdef HAS_DEVICE_SERIAL
		int i;
		extern void serial_printc(char);
		for (i = 0; i < len; i++)
		{
			serial_printc(((char *)buf)[i]);
		}
#else
		asm volatile(".byte 0x82"
					 : "=a"(len)
					 : "a"(4), "b"(fd), "c"(buf), "d"(len));
#endif
	}else{
		Fstate *f=&files[fd];
		if(f->used==false){
			Log("Writing to a not opened fd");
			return -1;
		}
		if(f->offset+len>file_table[f->index].size){
			Log("Writing more than file size, clipping");
			len=file_table[f->index].size-f->offset;
		}
		ide_write(buf, file_table[f->index].disk_offset+f->offset, len);
		f->offset+=len;
	}

	return len;
}

void check_boundary(int fd, off_t *offset){
	Fstate *f=&files[fd];
	const file_info *finfo=&file_table[f->index];

	if(*offset>finfo->disk_offset){
		Log("Seeking past end of file");
		*offset=finfo->size;
	}else if(*offset<0){
		Log("Seeking before start of file");
		*offset=0;
	}
}

off_t fs_lseek(int fd, off_t offset, int whence)
{
	if(fd<=2) return offset;
	Fstate *f=&files[fd];
	if(f->used==false){
		Log("Seeking a not opened file");
		return -1;
	}
	off_t new_off;
	switch (whence) {
		case SEEK_CUR:
			new_off=f->offset+offset;
		  // Log("relative seek");
			break;
		case SEEK_END:
			new_off=file_table[f->index].size+offset;
		  // Log("end seek");
			break;
		case SEEK_SET:
			new_off=offset;
			// Log("absolute seek to %d",new_off);
			break;
  }

	check_boundary(fd, &new_off);
	f->offset=new_off;
	return new_off;
}

int fs_close(int fd)
{
	if(fd<=2) return 0;
	Fstate *f=&files[fd];
	if(f->used==false){
		Log("Closing a not opened file");
		return -1;
	}
	f->used=true;
	return 0;
}
