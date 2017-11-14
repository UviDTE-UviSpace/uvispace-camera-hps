//file: avalon_image_writer_regs.h
//Register map of the avalon_image_writer component

#ifndef __AVALON_IMAGE_WRITER_REGS_H
#define __AVALON_IMAGE_WRITER_REGS_H

// Write 1 followed by a 0 to start new capture
#define START_CAPTURE         0x00
// Image width in pixels (a RGB pixel is considered only one pixel) typ. 640
#define CAPTURE_WIDTH         0x04
// Image height. typically 480
#define CAPTURE_HEIGHT        0x08
// Address in processor memory of the buffer 0
#define CAPTURE_BUFF0         0x0C
// Address in processor memory of the buffer 1
#define CAPTURE_BUFF1         0x10
// Choose buff0 (BUFFER_WRITE=0) or buff1 (BUFFER_WRITE=1) to write next image
#define CAPTURE_BUFFER_SELECT 0x14
// Image writer is idle and can capture a new image
// (it can be used to detect the end of capture)
#define CAPTURE_STANDBY       0x18
// Downsampling rate (1 = full image, 2 = 1/2 of rows and columns so size
// is reduced by 4, 2 = 1/4 of rows and columns are written so size
// is reduced 16 times, etc.)
#define CAPTURE_DOWNSAMPLING  0x1C
// Number of the last image captured (reset when only when the component
// receives a hardware reset)
#define CAPTURE_IMAGE_COUNTER 0x20

#endif //__AVALON_IMAGE_WRITER_REGS_H
