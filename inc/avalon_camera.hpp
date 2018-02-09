// file: avalon_camera.hpp
// It the avalon_camera component

#ifndef __AVALON_CAMERA_H
#define __AVALON_CAMERA_H

#include <inttypes.h> // for uint16_t
#include <string.h> // for memcpy

// Macros for accessing an address map
#include "hw_io.hpp"
// Internal address map of avalon camera (32-bit addresses)
#include "avalon_camera_regs.h"

/*
  Default values of some config registers
*/
#define CONFIG_WIDTH_DEFAULT 640
#define CONFIG_HEIGHT_DEFAULT 480
#define CONFIG_START_ROW_DEFAULT 0
#define CONFIG_START_COLUMN_DEFAULT 0
#define CONFIG_ROW_SIZE_DEFAULT 1919
#define CONFIG_COLUMN_SIZE_DEFAULT 2559
#define CONFIG_ROW_MODE_DEFAULT 17
#define CONFIG_COLUMN_MODE_DEFAULT 17
#define CONFIG_EXPOSURE_DEFAULT 1023
#define CONFIG_H_BLANKING_DEFAULT 0
#define CONFIG_V_BLANKING_DEFAULT 25
#define CONFIG_RED_GAIN_DEFAULT 156
#define CONFIG_BLUE_GAIN_DEFAULT 154
#define CONFIG_GREEN1_GAIN_DEFAULT 19
#define CONFIG_GREEN2_GAIN_DEFAULT 19

/*
  Class definition for easy control of the camera
*/
class Camera {
  private: // accesible only inside the class
    // --Class Variables--//
    // Virtual base address of the avalon_camera. Filled in the constructor
    void* address;

  public: // accessible from outside the class
    // --Class Methods definition--//
    // constructor
    Camera(void* virtual_address);

    // methods to set the camera configuration
    // the following methods change the values of the avalon_camera
    // registers without resetting the camera. So after using this
    // functions call config_update to reset the camera with the
    // new parameters and actually change the camera behaviour.
    int config_set_width(uint16_t val);
    int config_set_height(uint16_t val);
    int config_set_start_row(uint16_t val);
    int config_set_start_column(uint16_t val);
    int config_set_row_size(uint16_t val);
    int config_set_column_size(uint16_t val);
    int config_set_row_mode(uint16_t val);
    int config_set_column_mode(uint16_t val);
    int config_set_exposure(uint16_t val);
    int config_set_h_blanking(uint16_t val);
    int config_set_v_blanking(uint16_t val);
    int config_set_red_gain(uint16_t val);
    int config_set_blue_gain(uint16_t val);
    int config_set_green1_gain(uint16_t val);
    int config_set_green2_gain(uint16_t val);
    int config_set_default(void);
    // config_update loads new configuration into the camera and resets
    // the video stream.
    int config_update(void);

    // methods to get the camera configuration
    uint16_t config_get_width(void);
    uint16_t config_get_height(void);
    uint16_t config_get_start_row(void);
    uint16_t config_get_start_column(void);
    uint16_t config_get_row_size(void);
    uint16_t config_get_column_size(void);
    uint16_t config_get_row_mode(void);
    uint16_t config_get_column_mode(void);
    uint16_t config_get_exposure(void);
    uint16_t config_get_h_blanking(void);
    uint16_t config_get_v_blanking(void);
    uint16_t config_get_red_gain(void);
    uint16_t config_get_blue_gain(void);
    uint16_t config_get_green1_gain(void);
    uint16_t config_get_green2_gain(void);

  private: // not accesible from ouside the class
    // resets and removes soft reset to reset the video stream
    // it is private. not intended to be used by the user yet
    int reset(void);
};

// --Class Methods implementation --//

// class constructor (called when object is created)
Camera::Camera(void* virtual_address) {
  this->address = virtual_address;
  this->config_set_default();
  this->config_update();
}

// methods to set the camera configuration
int Camera::config_set_width(uint16_t val) {
  IOWR32(this->address, ADDR_WIDTH, val);
  return 0;
}
int Camera::config_set_height(uint16_t val) {
  IOWR32(this->address, ADDR_HEIGHT, val);
  return 0;
}
int Camera::config_set_start_row(uint16_t val) {
  IOWR32(this->address, ADDR_START_ROW, val);
  return 0;
}
int Camera::config_set_start_column(uint16_t val) {
  IOWR32(this->address, ADDR_START_COLUMN, val);
  return 0;
}
int Camera::config_set_row_size(uint16_t val) {
  IOWR32(this->address, ADDR_ROW_SIZE, val);
  return 0;
}
int Camera::config_set_column_size(uint16_t val) {
  IOWR32(this->address, ADDR_COLUMN_SIZE, val);
  return 0;
}
int Camera::config_set_row_mode(uint16_t val) {
  IOWR32(this->address, ADDR_ROW_MODE, val);
  return 0;
}
int Camera::config_set_column_mode(uint16_t val) {
  IOWR32(this->address, ADDR_COLUMN_MODE, val);
  return 0;
}
int Camera::config_set_exposure(uint16_t val) {
  IOWR32(this->address, ADDR_EXPOSURE, val);
  return 0;
}
int Camera::config_set_h_blanking(uint16_t val) {
  IOWR32(this->address, ADDR_H_BLANKING, val);
  return 0;
}
int Camera::config_set_v_blanking(uint16_t val) {
  IOWR32(this->address, ADDR_V_BLANKING, val);
  return 0;
}
int Camera::config_set_red_gain(uint16_t val) {
  IOWR32(this->address, ADDR_RED_GAIN, val);
  return 0;
}
int Camera::config_set_blue_gain(uint16_t val) {
  IOWR32(this->address, ADDR_BLUE_GAIN, val);
  return 0;
}
int Camera::config_set_green1_gain(uint16_t val) {
  IOWR32(this->address, ADDR_GREEN1_GAIN, val);
  return 0;
}
int Camera::config_set_green2_gain(uint16_t val) {
  IOWR32(this->address, ADDR_GREEN2_GAIN, val);
  return 0;
}
int Camera::config_set_default(void) {
  this->config_set_width(CONFIG_WIDTH_DEFAULT);
  this->config_set_height(CONFIG_HEIGHT_DEFAULT);
  this->config_set_start_row(CONFIG_START_ROW_DEFAULT);
  this->config_set_start_column(CONFIG_START_COLUMN_DEFAULT);
  this->config_set_row_size(CONFIG_ROW_SIZE_DEFAULT);
  this->config_set_column_size(CONFIG_COLUMN_SIZE_DEFAULT);
  this->config_set_row_mode(CONFIG_ROW_MODE_DEFAULT);
  this->config_set_column_mode(CONFIG_COLUMN_MODE_DEFAULT);
  this->config_set_exposure(CONFIG_EXPOSURE_DEFAULT);
  this->config_set_h_blanking(CONFIG_H_BLANKING_DEFAULT);
  this->config_set_v_blanking(CONFIG_V_BLANKING_DEFAULT);
  this->config_set_red_gain(CONFIG_RED_GAIN_DEFAULT);
  this->config_set_blue_gain(CONFIG_BLUE_GAIN_DEFAULT);
  this->config_set_green1_gain(CONFIG_GREEN1_GAIN_DEFAULT);
  this->config_set_green2_gain(CONFIG_GREEN2_GAIN_DEFAULT);
  return 0;
}
int Camera::config_update(void) {
  // this function is equal to reset now but in the future
  // could not be. So use this to update the camera config.
  this->reset();
  return 0;
}

// Methods to get the camera configuration
uint16_t Camera::config_get_width(void) {
  return IORD32(this->address, ADDR_WIDTH);
}
uint16_t Camera::config_get_height(void) {
  return IORD32(this->address, ADDR_HEIGHT);
}
uint16_t Camera::config_get_start_row(void) {
  return IORD32(this->address, ADDR_START_ROW);
}
uint16_t Camera::config_get_start_column(void) {
  return IORD32(this->address, ADDR_START_COLUMN);
}
uint16_t Camera::config_get_row_size(void) {
  return IORD32(this->address, ADDR_ROW_SIZE);
}
uint16_t Camera::config_get_column_size(void) {
  return IORD32(this->address, ADDR_COLUMN_SIZE);
}
uint16_t Camera::config_get_row_mode(void) {
  return IORD32(this->address, ADDR_ROW_MODE);
}
uint16_t Camera::config_get_column_mode(void) {
  return IORD32(this->address, ADDR_COLUMN_MODE);
}
uint16_t Camera::config_get_exposure(void) {
  return IORD32(this->address, ADDR_EXPOSURE);
}
uint16_t Camera::config_get_h_blanking(void) {
  return IORD32(this->address, ADDR_H_BLANKING);
}
uint16_t Camera::config_get_v_blanking(void) {
  return IORD32(this->address, ADDR_V_BLANKING);
}
uint16_t Camera::config_get_red_gain(void) {
  return IORD32(this->address, ADDR_RED_GAIN);
}
uint16_t Camera::config_get_blue_gain(void) {
  return IORD32(this->address, ADDR_BLUE_GAIN);
}
uint16_t Camera::config_get_green1_gain(void) {
  return IORD32(this->address, ADDR_GREEN1_GAIN);
}
uint16_t Camera::config_get_green2_gain(void) {
  return IORD32(this->address, ADDR_GREEN2_GAIN);
}

// reset
int Camera::reset(void) {
  // soft_reset is active low so when 0 the camera is reset
  IOWR32(this->address, CAMERA_SOFT_RESET, 0); // reset
  IOWR32(this->address, CAMERA_SOFT_RESET, 1); // remove reset
  return 0; // return 0 on success
}
#endif // __AVALON_CAMERA_H
