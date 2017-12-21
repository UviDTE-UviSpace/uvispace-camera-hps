//file: avalon_image_processing_regs.h
//Register map of the avalon_image_processing

#ifndef __AVALON_IMAGE_PROCESSING_REGS_H
#define __AVALON_IMAGE_PROCESSING_REGS_H

// Registers to control avalon_image_processing component
  //Binarization thresholds for the hsv2bin component
#define ADDR_HUE_THRESHOLD_L         0x00
#define ADDR_HUE_THRESHOLD_H         0x04
#define ADDR_BRI_THRESHOLD_L         0x08
#define ADDR_BRI_THRESHOLD_H         0x0C
#define ADDR_SAT_THRESHOLD_L         0x10
#define ADDR_SAT_THRESHOLD_H         0x14

#endif //__AVALON_IMAGE_PROCESSING_REGS_H
