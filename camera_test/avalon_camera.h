#ifndef __AVALON_CAMERA_H
#define __AVALON_CAMERA_H

#include <inttypes.h>//for uint16_t

/* 
Macros for accessing an address map with a data bus size of 8, 16 or 32 bits.
*/ 
#ifndef __LOW_LEVEL_RW_MACROS
#define __LOW_LEVEL_RW_MACROS
// Macro for R/W operations on 8-bit addresses.
#define IOWR8(base, offset, dat)        (*((uint8_t*)base + offset) = (uint8_t)dat)
#define IORD8(base, offset)             (*((uint8_t*)base + offset))
// Macro for R/W operations on 16-bit addresses.
#define IOWR16(base, offset, dat)       ((* ((uint32_t*)((void*)base + offset*2))) = (uint16_t)dat)
#define IORD16(base, offset)            (* ((uint32_t*)((void*)base + offset*2)))
// Macro for R/W operations on 16-bit addresses.
#define IOWR32(base, offset, dat)       ((* ((uint32_t*)((void*)base + offset*4))) = (uint32_t)dat)
#define IORD32(base, offset)            (* ((uint32_t*)((void*)base + offset*4)))
#endif

/* 
Internal address map and macro functions for control registers
*/ 
// 32-bit addresses
#define CAPTURE_START           0x00
#define CAPTURE_CONFIGURE       0x01
#define CAPTURE_SELECT_VGA      0x02
#define CAPTURE_SELECT_SENSOR   0x03
#define CAPTURE_DATA            0x04
#define CAMERA_SOFT_RESET       0x05

#define IOWR_CAMERA_CAPTURE_START(base)                 IOWR32(base, CAPTURE_START, 1)
#define IOWR_CAMERA_CAPTURE_STOP(base)                  IOWR32(base, CAPTURE_START, 0)
#define IORD_CAMERA_CAPTURE_DONE(base)                  IORD32(base, CAPTURE_START)
#define IOWR_CAMERA_CONFIGURE(base, dat)                IOWR32(base, CAPTURE_CONFIGURE, dat)
#define IORD_CAMERA_READY(base)                         IORD32(base, CAPTURE_CONFIGURE)
#define IOWR_CAMERA_CAPTURE_SELECT_VGA(base, dat)       IOWR32(base, CAPTURE_SELECT_VGA, dat)
#define IOWR_CAMERA_CAPTURE_SELECT_SENSOR(base, dat)    IOWR32(base, CAPTURE_SELECT_SENSOR, dat)
#define IORD_CAMERA_READ_DATA(base)                     IORD32(base, CAPTURE_DATA)
#define IOWR_CAMERA_CAMERA_SOFT_RESET(base, dat)        IOWR32(base, CAMERA_SOFT_RESET, dat)
#define IORD_CAMERA_CAMERA_SOFT_RESET(base)             IORD32(base, CAMERA_SOFT_RESET)


/* 
Internal address map and macro functions for control registers
*/ 
// 32-bit addresses
#define ADDR_WIDTH          0x08
#define ADDR_HEIGHT         0x0a
#define ADDR_START_ROW      0x0c
#define ADDR_START_COLUMN   0x0e
#define ADDR_ROW_SIZE       0x10
#define ADDR_COLUMN_SIZE    0x12
#define ADDR_ROW_MODE       0x14
#define ADDR_COLUMN_MODE    0x16
#define ADDR_EXPOSURE       0x18

#define IORD_CAMERA_WIDTH(base)             IORD32(base, ADDR_WIDTH)
#define IOWR_CAMERA_WIDTH(base, dat)        IOWR32(base, ADDR_WIDTH, dat)
#define IORD_CAMERA_HEIGHT(base)            IORD32(base, ADDR_HEIGHT)
#define IOWR_CAMERA_HEIGHT(base, dat)       IOWR32(base, ADDR_HEIGHT, dat)

#define IORD_CAMERA_START_ROW(base)         IORD32(base, ADDR_START_ROW)
#define IOWR_CAMERA_START_ROW(base, dat)    IOWR32(base, ADDR_START_ROW, dat)
#define IORD_CAMERA_START_COLUMN(base)      IORD32(base, ADDR_START_COLUMN)
#define IOWR_CAMERA_START_COLUMN(base, dat) IOWR32(base, ADDR_START_COLUMN, dat)

#define IORD_CAMERA_ROW_SIZE(base)          IORD32(base, ADDR_ROW_SIZE)
#define IOWR_CAMERA_ROW_SIZE(base, dat)     IOWR32(base, ADDR_ROW_SIZE, dat)
#define IORD_CAMERA_COLUMN_SIZE(base)       IORD32(base, ADDR_COLUMN_SIZE)
#define IOWR_CAMERA_COLUMN_SIZE(base, dat)  IOWR32(base, ADDR_COLUMN_SIZE, dat)

#define IORD_CAMERA_ROW_MODE(base)          IORD32(base, ADDR_ROW_MODE)
#define IOWR_CAMERA_ROW_MODE(base, dat)     IOWR32(base, ADDR_ROW_MODE, dat)
#define IORD_CAMERA_COLUMN_MODE(base)       IORD32(base, ADDR_COLUMN_MODE)
#define IOWR_CAMERA_COLUMN_MODE(base, dat)  IOWR32(base, ADDR_COLUMN_MODE, dat)

#define IORD_CAMERA_EXPOSURE(base)           IORD32(base, ADDR_EXPOSURE)
#define IOWR_CAMERA_EXPOSURE(base, dat)      IOWR32(base, ADDR_EXPOSURE, dat)

/* 
Higher level functions using the macro functions
*/ 
int camera_set_exposure(uint16_t exposure);
int camera_generate_soft_reset(void);
int camera_init(void* camera_address);
#endif
