//file: avalon_camera_regs.h
//Register map of the avalon_camera component

#ifndef __AVALON_CAMERA_REGS_H
#define __AVALON_CAMERA_REGS_H

// Registers to control camera_config component
#define ADDR_WIDTH                     0x00
#define ADDR_HEIGHT                    0x04
#define ADDR_START_ROW                 0x08
#define ADDR_START_COLUMN              0x0C
#define ADDR_ROW_SIZE                  0x10
#define ADDR_COLUMN_SIZE               0x14
#define ADDR_ROW_MODE                  0x18
#define ADDR_COLUMN_MODE               0x1C
#define ADDR_EXPOSURE                  0x20
#define ADDR_H_BLANKING                0x24
#define ADDR_V_BLANKING                0x28
#define ADDR_RED_GAIN                  0x2C
#define ADDR_BLUE_GAIN                 0x30
#define ADDR_GREEN1_GAIN               0x34
#define ADDR_GREEN2_GAIN               0x38
// soft_reset signal
#define CAMERA_SOFT_RESET              0x7C

#endif //__AVALON_CAMERA_REGS_H
