//file: avalon_image_writer_regs.h
//Register map of the avalon_image_writer component

#ifndef __AVALON_IMAGE_WRITER_REGS_H
#define __AVALON_IMAGE_WRITER_REGS_H

// Acquisition mode. Write 0 = SINGLE_SHOT mode (for taking a pic) and
// 1 = CONTINUOUS mode (continuously saving images in memory, better for video)
#define CAPTURE_MODE          0x00
// Image width in pixels (a RGB pixel is considered only one pixel) typ. 640
#define CAPTURE_WIDTH         0x04
// Image height. typically 480
#define CAPTURE_HEIGHT        0x08
// Address in processor memory of the buffer 0
#define CAPTURE_BUFF0         0x0C
// Address in processor memory of the buffer 1
#define CAPTURE_BUFF1         0x10
// Used only in CONTINUOUS mode. 0 = use one buffer, 1 = use 2 buffers
// and alternate between them ignoring CAPTURE_BUFFER_SELECT.
// The first image always go to buff0, second to buff1, then buff0 and so on...
#define CONT_DOUBLE_BUFF      0x14
// Choose buff0 (0) or buff1 (1) to write next image/s. Only applies in
// SINGLE_SHOT mode or in CONTINUOUS mode with CONT_DOUBLE_BUFF=0
#define CAPTURE_BUFFER_SELECT 0x18
// In SINGLE_SHOT write 1 here to start a capture. It goes to 0 automatically
// In CONTINUOUS MODE write 1 to start continuous capture of images into
// memory and write a 0 to stop it.
#define START_CAPTURE         0x1C
// Image writer is idle and can capture a new image
// (it can be used to detect the end of capture in SINGLE_SHOT mode)
#define CAPTURE_STANDBY       0x20
// Shows where the last image was saved: buff0 (0) or buff1(1). Useful in
// CONTINUOUS mode to know where the newest img is (0 for buff 0, 1 for buff1)
#define LAST_BUFFER_CAPTURED  0x24
// Downsampling rate (1 = full image, 2 = 1/2 of rows and columns so size
// is reduced by 4, 2 = 1/4 of rows and columns are written so size
// is reduced 16 times, etc.)
#define CAPTURE_DOWNSAMPLING  0x28
// Number of the image being acquired now. It resets only when the component is reset
// and it counts regardless the component is capturing images or not.
#define CAPTURE_IMAGE_COUNTER 0x2C

#endif //__AVALON_IMAGE_WRITER_REGS_H
