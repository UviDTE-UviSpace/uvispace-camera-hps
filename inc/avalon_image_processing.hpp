// file: avalon_camera.hpp
// It the avalon_camera component

#ifndef __AVALON_CAMERA_H
#define __AVALON_CAMERA_H

#include <inttypes.h> // for uint16_t
#include <string.h> // for memcpy

// Macros for accessing an address map
#include "hw_io.hpp"
// Internal address map of avalon camera (32-bit addresses)
#include "avalon_image_processing_regs.h"

/*
  Default values of some config registers
*/
#define HUE_THRESHOLD_L_DEFAULT 220
#define HUE_THRESHOLD_H_DEFAULT 30
#define BRI_THRESHOLD_L_DEFAULT 60
#define SAT_THRESHOLD_L_DEFAULT 60

/*
  Class definition for easy control of the camera
*/
class ImageProcessing {
  private: // accesible only inside the class
    // --Class Variables--//
    // Virtual base address of the avalon_image_processing.
    void* address;

  public: // accessible from outside the class
    // --Class Methods definition--//
    // constructor
    ImageProcessing(void* virtual_address);

    // Methods to set the thresholds for hsv 2 binary conversion
    int set_hue_ths_L(uint8_t val);
    int set_hue_th_H(uint8_t val);
    int set_brightness_th_H(uint8_t val);
    int set_saturation_th_H(uint8_t val);

    // Method to set default parameters for image img_processing
    int set_default(void);

    // Methods to set the thresholds for hsv 2 binary conversion
    uint8_t get_hue_ths_L(void);
    uint8_t get_hue_th_H(void);
    uint8_t get_brightness_th_H(void);
    uint8_t get_saturation_th_H(void);
};

// --Class Methods implementation --//

// class constructor (called when object is created)
ImageProcessing::ImageProcessing(void* virtual_address) {
  this->address = virtual_address;
  this->set_default();
}

// Methods to set the thresholds for hsv 2 binary conversion
int ImageProcessing::set_hue_ths_L(uint8_t val){
  IOWR32(this->address, ADDR_HUE_THRESHOLD_L, val);
  return 0;
}
int ImageProcessing::set_hue_th_H(uint8_t val){
  IOWR32(this->address, ADDR_HUE_THRESHOLD_H, val);
  return 0;
}
int ImageProcessing::set_brightness_th_H(uint8_t val){
  IOWR32(this->address, ADDR_BRI_THRESHOLD_L, val);
  return 0;
}
int ImageProcessing::set_saturation_th_H(uint8_t val){
  IOWR32(this->address, ADDR_SAT_COLUMN, val);
  return 0;
}

// Method to set default parameters for image img_processing
int ImageProcessing::set_default(void) {
  this->set_hue_ths_L(HUE_THRESHOLD_L_DEFAULT);
  this->set_hue_th_H(HUE_THRESHOLD_H_DEFAULT);
  this->set_brightness_th_H(BRI_THRESHOLD_L_DEFAULT);
  this->set_saturation_th_H(SAT_THRESHOLD_L_DEFAULT);
  return 0;
}

// Methods to set the thresholds for hsv 2 binary conversion
uint8_t ImageProcessing::get_hue_ths_L(void){
  return IORD32(this->address, ADDR_HUE_THRESHOLD_L);
}
uint8_t ImageProcessing::get_hue_th_H(void){
  return IORD32(this->address, ADDR_HUE_THRESHOLD_H);
}
uint8_t ImageProcessing::get_brightness_th_H(void){
  return IORD32(this->address, ADDR_BRI_THRESHOLD_L);
}
uint8_t ImageProcessing::get_saturation_th_H(void){
  return IORD32(this->address, ADDR_SAT_COLUMN);
}
#endif // __AVALON_CAMERA_H
