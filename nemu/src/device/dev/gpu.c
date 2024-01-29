#include "device/gpu.h"
#include "cpu/reg.h"
#include "memory/memory.h"
#include "nemu.h"
#include "debug.h"
#include <SDL/SDL_video.h>
#include <stddef.h>
#include <stdint.h>

#ifdef HAS_DEVICE_GPU

void gpu_init(){
  
}

static union{
  uint32_t vaddr;
  uint8_t addr_bytes[4];
} addr;

make_pio_handler(gpu_data_handler){
  if(is_write==false){
    printf("ERROR: Reading from GPU data port\n");
    return;
  }
  assert(port>=GPU_DATA_PORT_BASE && port<GPU_DATA_PORT_BASE+4);
  int index=port-GPU_DATA_PORT_BASE;
  addr.addr_bytes[index]=read_io_port(port, 1);
}

// Load data from virtual machine's memory
// addr: virtual memory in VM
#define LOAD(x,addr)                         \
{                                            \
  printf("Reading " #x " from 0x%x\n",addr); \
  char *w=(char*)&(x);                       \
  for(int i=0;i<sizeof(x);i++){              \
    w[i]=vaddr_read((addr)+i, SREG_DS, 1);   \
  }                                          \
}                                            \

make_pio_handler(gpu_cmd_handler){
  if(is_write==false){
    printf("ERROR: Reading from GPU cmd port");
    return;
  }
  // Flat mode, seg doesn't matter
  GPU_CMD cmd;
  LOAD(cmd, addr.vaddr);
  hexdump(cmd);

  SDL_Surface srcs, dsts;
  SDL_Rect srcr, dstr;
  LOAD(srcs, cmd.src_surface);
  LOAD(dsts, cmd.dst_surface);
  LOAD(srcr, cmd.src_rect);
  LOAD(dstr, cmd.dst_rect);
  hexdump(cmd);

  for(int y=srcr.y;y<srcr.y+srcr.h;y++){
    for(int x=srcr.x;x<srcr.x+srcr.w;x++){
      printf("gpu_blit: srcs.pixels=0x%x, y=%d, srcs.w=%d, x=%d, addr=0x%x\n",
              srcs.pixels, y, srcs.w, x, (srcs.pixels+y*srcs.w+x));
      uint8_t data=vaddr_read((vaddr_t)(srcs.pixels+y*srcs.w+x), SREG_DS, 1);
      vaddr_write((vaddr_t)(dsts.pixels+y*dsts.w+x), SREG_DS, 1, data);
    }
  }
}

#endif
