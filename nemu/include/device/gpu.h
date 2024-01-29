#ifndef __GPU_H__
#define __GPU_H__

#include "nemu.h"
#include "device/port_io.h"
#include "cpu/reg.h"
#include "device/port_io.h"
#include "memory/mmu/page.h"
#include "memory/mmu/segment.h"
#include "memory.h"
#include "nemu.h"
#include <stdint.h>

#define GPU_DATA_PORT_BASE 0x5da
#define GPU_CMD_PORT 0x5de

#define BLIT_CMD 0x01

void gpu_init();
make_pio_handler(gpu_data_handler);
make_pio_handler(gpu_cmd_handler);


#endif
