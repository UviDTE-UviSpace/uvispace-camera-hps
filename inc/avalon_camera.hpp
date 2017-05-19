//file: avalon_camera.hpp
//It the avalon_camera component

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
#define IOWR16(base, offset, dat)       ((* ((uint32_t*)((uint8_t*)base + offset*2))) = (uint16_t)dat)
#define IORD16(base, offset)            (* ((uint32_t*)((uint8_t)base + offset*2)))
// Macro for R/W operations on 16-bit addresses.
#define IOWR32(base, offset, dat)       ((* ((uint32_t*)((uint8_t*)base + offset*4))) = (uint32_t)dat)
#define IORD32(base, offset)            (* ((uint32_t*)((uint8_t*)base + offset*4)))
#endif

/* 
Internal address map of avalon camera (32-bit addresses)
*/ 
// registers to control image_capture component
#define CAMERA_START_CAPTURE           0x00
#define CAMERA_CAPTURE_WIDTH           0x01
#define CAMERA_CAPTURE_HEIGHT          0x02
#define CAMERA_BUFF0                   0x03
#define CAMERA_BUFF1                   0x04
#define CAMERA_BUFF0_FULL              0x05
#define CAMERA_BUFF1_FULL              0x06
// registers to control camera_config component
#define ADDR_WIDTH                     0x09
#define ADDR_HEIGHT                    0x0a
#define ADDR_START_ROW                 0x0b
#define ADDR_START_COLUMN              0x0c
#define ADDR_ROW_SIZE                  0x0d
#define ADDR_COLUMN_SIZE               0x0e
#define ADDR_ROW_MODE                  0x0f
#define ADDR_COLUMN_MODE               0x10
#define ADDR_EXPOSURE                  0x11
// soft_reset signal
#define CAMERA_SOFT_RESET              0x1f

/* 
Default values of some config registers
*/ 
#define CONFIG_WIDTH_DEFAULT                     640
#define CONFIG_HEIGHT_DEFAULT                    480
#define CONFIG_START_ROW_DEFAULT                 0
#define CONFIG_START_COLUMN_DEFAULT              0
#define CONFIG_ROW_SIZE_DEFAULT                  1919
#define CONFIG_COLUMN_SIZE_DEFAULT               2559
#define CONFIG_ROW_MODE_DEFAULT                  17
#define CONFIG_COLUMN_MODE_DEFAULT               17
#define CONFIG_EXPOSURE_DEFAULT                  1984

/* 
Macro functions for RD/WR the registers
*/ 
//
#define IOWR_CAMERA_START_CAPTURE(base, dat)    IOWR32(base, CAMERA_START_CAPTURE, dat)
#define IORD_CAMERA_START_CAPTURE(base)         IORD32(base, CAMERA_START_CAPTURE)
#define IOWR_CAMERA_BUFF0(base, dat)            IOWR32(base, CAMERA_BUFF0, dat)
#define IORD_CAMERA_BUFF0(base)                 IORD32(base, CAMERA_BUFF0)
#define IOWR_CAMERA_BUFF1(base, dat)            IOWR32(base, CAMERA_BUFF1, dat)
#define IORD_CAMERA_BUFF1(base)                 IORD32(base, CAMERA_BUFF1)
#define IOWR_CAMERA_BUFF0FULL(base, dat)        IOWR32(base, CAMERA_BUFF0_FULL, dat)
#define IORD_CAMERA_BUFF0FULL(base)             IORD32(base, CAMERA_BUFF0_FULL)
#define IOWR_CAMERA_BUFF1FULL(base, dat)        IOWR32(base, CAMERA_BUFF1_FULL, dat)
#define IORD_CAMERA_BUFF1FULL(base)             IORD32(base, CAMERA_BUFF1_FULL)
//
#define IORD_CAMERA_WIDTH(base)                 IORD32(base, ADDR_WIDTH)
#define IOWR_CAMERA_WIDTH(base, dat)            IOWR32(base, ADDR_WIDTH, dat)
#define IORD_CAMERA_HEIGHT(base)                IORD32(base, ADDR_HEIGHT)
#define IOWR_CAMERA_HEIGHT(base, dat)           IOWR32(base, ADDR_HEIGHT, dat)
#define IORD_CAMERA_START_ROW(base)             IORD32(base, ADDR_START_ROW)
#define IOWR_CAMERA_START_ROW(base, dat)        IOWR32(base, ADDR_START_ROW, dat)
#define IORD_CAMERA_START_COLUMN(base)          IORD32(base, ADDR_START_COLUMN)
#define IOWR_CAMERA_START_COLUMN(base, dat)     IOWR32(base, ADDR_START_COLUMN, dat)
#define IORD_CAMERA_ROW_SIZE(base)              IORD32(base, ADDR_ROW_SIZE)
#define IOWR_CAMERA_ROW_SIZE(base, dat)         IOWR32(base, ADDR_ROW_SIZE, dat)
#define IORD_CAMERA_COLUMN_SIZE(base)           IORD32(base, ADDR_COLUMN_SIZE)
#define IOWR_CAMERA_COLUMN_SIZE(base, dat)      IOWR32(base, ADDR_COLUMN_SIZE, dat)
#define IORD_CAMERA_ROW_MODE(base)              IORD32(base, ADDR_ROW_MODE)
#define IOWR_CAMERA_ROW_MODE(base, dat)         IOWR32(base, ADDR_ROW_MODE, dat)
#define IORD_CAMERA_COLUMN_MODE(base)           IORD32(base, ADDR_COLUMN_MODE)
#define IOWR_CAMERA_COLUMN_MODE(base, dat)      IOWR32(base, ADDR_COLUMN_MODE, dat)
#define IORD_CAMERA_EXPOSURE(base)              IORD32(base, ADDR_EXPOSURE)
#define IOWR_CAMERA_EXPOSURE(base, dat)         IOWR32(base, ADDR_EXPOSURE, dat)
//
#define IORD_CAMERA_SOFT_RESET(base)            IORD32(base, CAMERA_SOFT_RESET)
#define IOWR_CAMERA_SOFT_RESET(base, dat)       IOWR32(base, CAMERA_SOFT_RESET, dat)

/* 
Class definition for easy control of the camera
*/
class Camera
{
private: //accesible only inside the class
    //--Class Variables--//
    //Virtual base address of the avalon_camera. Filled in the constructor
    void* address;
public: //accessible from outside the class
    //--Class Methods definition--//
    //constructor 
	Camera(void* virtual_address);
    
    //methods to set the camera configuration
    //the following methods change the values of the avalon_camera
    //registers without resetting the camera. So after using this 
    //functions call update_config to reset the camera with the 
    //new parameters and actually change the camera behaviour.
    int config_set_width(uint16_t val);
    int config_set_height(uint16_t val);
    int config_set_start_row(uint16_t val);
    int config_set_start_column(uint16_t val);
    int config_set_row_size(uint16_t val);
    int config_set_column_size(uint16_t val);
    int config_set_row_mode(uint16_t val);
    int config_set_column_mode(uint16_t val);
    int config_set_exposure(uint16_t val);
    int set_default_config(void);
    //update_config loads new configuration into the camera and resets
    //the video stream.
    int update_config(void);
    
    //methods to get the camera configuration
    uint16_t config_get_width(void);
    uint16_t config_get_height(void);
    uint16_t config_get_start_row(void);
    uint16_t config_get_start_column(void);
    uint16_t config_get_row_size(void);
    uint16_t config_get_column_size(void);
    uint16_t config_get_row_mode(void);
    uint16_t config_get_column_mode(void);
    uint16_t config_get_exposure(void);
    
    //methods to capture an image into the processor
    
     
private: //accesible from ouside the class   
    //resets and removes soft reset to reset the video stream 
    //it is private. not intended to be used by the user yet
    int reset(void);
};

 //--Class Methods implementation --//
 
 //class constructor (called when object is created)
 Camera::Camera(void* virtual_address)
 {
     this->address = virtual_address;
     this->set_default_config();
     this->update_config();
 }
 
 //methods to set the camera configuration
 int Camera::config_set_width(uint16_t val) 
 {
     IOWR32(this->address, ADDR_WIDTH, val);
     return 0;
 }
 int Camera::config_set_height(uint16_t val) 
 {
     IOWR32(this->address, ADDR_HEIGHT, val);
     return 0;
 }
 int Camera::config_set_start_row(uint16_t val) 
 {
     IOWR32(this->address, ADDR_START_ROW, val);
     return 0;
 }
 int Camera::config_set_start_column(uint16_t val) 
 {
     IOWR32(this->address, ADDR_START_COLUMN, val);
     return 0;
 }
 int Camera::config_set_row_size(uint16_t val) 
 {
     IOWR32(this->address, ADDR_ROW_SIZE, val);
     return 0;
 }
 int Camera::config_set_column_size(uint16_t val) 
 {
     IOWR32(this->address, ADDR_COLUMN_SIZE, val);
     return 0;
 }
 int Camera::config_set_row_mode(uint16_t val) 
 {
     IOWR32(this->address, ADDR_ROW_MODE, val);
     return 0;
 }
 int Camera::config_set_column_mode(uint16_t val) 
 {
     IOWR32(this->address, ADDR_COLUMN_MODE, val);
     return 0;
 }
 int Camera::config_set_exposure(uint16_t val) 
 {
     IOWR32(this->address, ADDR_EXPOSURE, val);
     return 0;
 }
 int Camera::set_default_config(void) 
 {
     this->config_set_width(CONFIG_WIDTH_DEFAULT);
     this->config_set_height(CONFIG_HEIGHT_DEFAULT);
     this->config_set_start_row(CONFIG_START_ROW_DEFAULT);
     this->config_set_start_column(CONFIG_START_COLUMN_DEFAULT);
     this->config_set_row_size(CONFIG_ROW_SIZE_DEFAULT) ;
     this->config_set_column_size(CONFIG_COLUMN_SIZE_DEFAULT); 
     this->config_set_row_mode(CONFIG_ROW_MODE_DEFAULT); 
     this->config_set_column_mode(CONFIG_COLUMN_MODE_DEFAULT);
     this->config_set_exposure(CONFIG_EXPOSURE_DEFAULT);
     return 0;
 }    
 int Camera::update_config(void)
 {
     //this function is equal to reset now but in the future 
     //could not be. So use this to update the camera config.
     this->reset();
 } 
 
 //Methods to get the camera configuration
 uint16_t Camera::config_get_width(void)
 {
     return IORD32(this->address, ADDR_WIDTH);
 }
 uint16_t Camera::config_get_height(void)
 {
     return IORD32(this->address, ADDR_HEIGHT);
 }
 uint16_t Camera::config_get_start_row(void)
 {
     return IORD32(this->address, ADDR_START_ROW);
 }
 uint16_t Camera::config_get_start_column(void)
 {
     return IORD32(this->address, ADDR_START_COLUMN);
 }
 uint16_t Camera::config_get_row_size(void)
 {
     return IORD32(this->address, ADDR_ROW_SIZE);
 }
 uint16_t Camera::config_get_column_size(void)
 {
     return IORD32(this->address, ADDR_COLUMN_SIZE);
 }
 uint16_t Camera::config_get_row_mode(void)
 {
     return IORD32(this->address, ADDR_ROW_MODE);
 }
 uint16_t Camera::config_get_column_mode(void)
 {
     return IORD32(this->address, ADDR_COLUMN_MODE);
 }
 uint16_t Camera::config_get_exposure(void)
 {
     return IORD32(this->address, ADDR_EXPOSURE);
 }
 
 
 //reset
 int Camera::reset(void)
 {
    //soft_reset is active low so when 0 the camera is reset
    IOWR32(this->address, CAMERA_SOFT_RESET, 0);//reset
    IOWR32(this->address, CAMERA_SOFT_RESET, 1);//remove reset
    return 0; //return 0 on success
 }

#endif //__AVALON_CAMERA_H
