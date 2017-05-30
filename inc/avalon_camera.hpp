//file: avalon_camera.hpp
//It the avalon_camera component

#ifndef __AVALON_CAMERA_H
#define __AVALON_CAMERA_H

#include <inttypes.h>//for uint16_t
#include <string.h>//for memcpy

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
#define CAMERA_CAPTURE_STANDBY         0x07
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
#define CONFIG_ROW_MODE_DEFAULT                  1
#define CONFIG_COLUMN_MODE_DEFAULT               17
#define CONFIG_EXPOSURE_DEFAULT                  1984

/*
Camera capture error codes
*/
#define CAMERA_NO_REPLY 1
#define CAMERA_CAPTURE_GET_LINE_BUFFER_FULL_NO_WAIT 2
#define CAMERA_CAPTURE_GET_LINE_TIMEOUT 3

/*
Struct to help the acquisition of the image
*/
// Size in bits of a color component (R,G,B or Gray)
// We have settled in a 8 bit component, in case a 16 bit component was required, the typedef should be
// changed to uint16_t.
typedef uint8_t color_component;

//format of a captured pixel
typedef struct cpixel_
{
    color_component R;
    color_component G;
    color_component B;
    color_component Gray;
}cpixel;

/*
Class definition for easy control of the camera
*/
class Camera
{
private: //accesible only inside the class
    //--Class Variables--//
    //Virtual base address of the avalon_camera. Filled in the constructor
    void* address;

    //Size of the image in pixels (screen pixels.
    //dont missunderstand with cam pixels that are usually double)
    //Filled each time config_set_width and config_set_height are called
    uint16_t img_width;
    uint16_t img_height;

    //virtual addresses of the buffers where images are captured.
    //virtual and physical addresses of the buffer that connects
    //hardware. Used to save lines of the image
    void* buff_v;
    void* buff_p;
    cpixel* buff0_v; //pointer inside buff_v, to save odd lines
    cpixel* buff1_v; //pointer inside buff_v, to save even lines
    //current_buff_v is sometimes buff0_v, sometimes buff1_v, depending
    //on the image line currently being acquired
    cpixel* current_buff_v;

public: //accessible from outside the class
    //--Class Methods definition--//
    //constructor
	Camera(void* virtual_address);

    //methods to set the camera configuration
    //the following methods change the values of the avalon_camera
    //registers without resetting the camera. So after using this
    //functions call config_update to reset the camera with the
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
    int config_set_default(void);
    //config_update loads new configuration into the camera and resets
    //the video stream.
    int config_update(void);

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
    int capture_set_buffer(void* buffer_v, void* buffer_p);
    int capture_get_image(cpixel* image);
private:
    int capture_start(void);
    int capture_get_line(cpixel*& line);

private: //not accesible from ouside the class
    //resets and removes soft reset to reset the video stream
    //it is private. not intended to be used by the user yet
    int reset(void);
};

 //--Class Methods implementation --//

 //class constructor (called when object is created)
 Camera::Camera(void* virtual_address)
 {
     this->address = virtual_address;
     this->config_set_default();
     this->config_update();
 }

 //methods to set the camera configuration
 int Camera::config_set_width(uint16_t val)
 {
     IOWR32(this->address, ADDR_WIDTH, val);
     this->img_width = val;
     return 0;
 }
 int Camera::config_set_height(uint16_t val)
 {
     IOWR32(this->address, ADDR_HEIGHT, val);
     this->img_height = val;
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
 int Camera::config_set_default(void)
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
 int Camera::config_update(void)
 {
     //this function is equal to reset now but in the future
     //could not be. So use this to update the camera config.
     this->reset();
     return 0;
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

 //--Capture image methods
 //capture_set_buffer
 //
 //description: sets the physical and virtual addresses of the hardware
 //buffer connecting hardware and software. This buffer is used to save
 //2 lines of the image. Since this buffer may not have enough space to
 //store a whole image (in example, 4 8-bit components of a 640x480 image
 //occupies 1.2MB while HPS-OCR has only 64kB) while one line is being
 //acquired by the hardware component capture_image, the other is being
 //copied to a buffer in processor RAM with enough space to store a whole
 //image.
 //
 //parameters:
 //-buffer_v: virtual address of the buffer to connect processor and fpga.
 // This buffer must have enough space to store 2 lines of the image,
 // that is: Minimum size = sizeof(cpixel)*this->img_width*2.
 // This buffer must be physically contiguous cause the hardware component
 // image_capture in will write continuously to it.
 //-buffer_p: equivalent physical address for the buffer.
 //
 //return: 0 if success
 int Camera::capture_set_buffer(void* buffer_v, void* buffer_p)
 {
    this->buff_v = buffer_v;
    this->buff_p = buffer_p;
    return 0;
 }

 //capture_get_image
 //
 //description
 //After configuring the camera and calling capture_set_buffer
 //the user can call this function to retrieve a whole image
 //from the video stream in the fpga. This function uses
 //capture_start() to start a new capture. Then uses capture_get_line()
 //to know when a new line has been acquired. When a new line is
 //acquired the processor copies its content into the image buffer,
 //passed as an argument to this function. When all lines are acquired
 //the function ends.
 //
 //parameters: image is a buffer with enough space to store a whole image.
 //            The image is stored by lines in this buffer, that is
 //            line0 goes first in the buffer, then line1 and so on.
 //
 //return: 0 success
 //        1 error starting the capture (component does not answer
 //          (it is in reset or not connected to the bus).
 //        2 error line already acquired when entering capture_get_line.
 //          Possible loss of data.
 //          The processor did too much in between two calls to
 //          capture_get_line. Try to reduce frame rate.
 //        3 Too much waiting for a new line. Looks like no video
 //          stream is there. Maybe the video stream is in reset state?
 int Camera::capture_get_image(cpixel* image)
 {
    int error;
    int i;
    cpixel* line;
    int line_size_B = this->img_width * sizeof(cpixel); //line size in Bytes

    //Start capture
    error = this->capture_start();
    if (error == CAMERA_NO_REPLY) return CAMERA_NO_REPLY;

    //Capture lines
    for(i=0; i<this->img_height; i++) //for every line
    {
        //wait till line is captured
        error = this->capture_get_line(line);
        if (error == CAMERA_CAPTURE_GET_LINE_BUFFER_FULL_NO_WAIT)
        {
            printf("Error in line %d\n", i);
            return CAMERA_CAPTURE_GET_LINE_BUFFER_FULL_NO_WAIT;
        }
        else if (error == CAMERA_CAPTURE_GET_LINE_TIMEOUT)
            return CAMERA_CAPTURE_GET_LINE_TIMEOUT;

        //Copy the line from buffer for lines to big buffer in processor
        //memcpy is the fastest method to copy data (for dma a driver is needed)
        //void * memcpy ( void * destination, const void * source, size_t num );
        memcpy ((void*)&image[i*this->img_width], (void*)line, line_size_B );
    }
    return 0;
 }

 //capture_start()
 //
 //description:
 //starts the acquisition of an image using image_capture
 //component. After capture_start is called use capture_get_line() to know
 //when a new line has been captured and where it is stored.
 //This function is "recall-safe". There is no need to wait until the end
 //of the capture to call capture_start() again.
 //The function will wait until the component enters in standby to
 //synchronize and start a new image from the beginning. So it is safe
 //to recall the function even if you don´t know the state of the previous
 //acquisition.
 //
 //
 // return: 0 success.
 //         1 error starting the capture (component does not answer
 //          (it is in reset or not connected to the bus).

 int Camera::capture_start(void)
 {
    //generate the software addresses of buff0 and buff1
    this->buff0_v = (cpixel*) this->buff_v;
    this->buff1_v = (cpixel*)((uint8_t*)this->buff_v
                    + sizeof(cpixel)*this->img_width);
    //First line will be saved in buff0
    this->current_buff_v = this->buff0_v;

    //generate physical addresses and save the into the avalon_camera
    //so image capture knows physical addresses of buff0 and buff1
    void* buff0_p = this->buff_p;
    void* buff1_p = (void*)((uint8_t*)this->buff_p
                    + sizeof(cpixel)*this->img_width);
    IOWR32(this->address, CAMERA_BUFF0, buff0_p);
    IOWR32(this->address, CAMERA_BUFF1, buff1_p);

    //Indicate the image size to the capture_image component
    IOWR32(this->address, CAMERA_CAPTURE_WIDTH, this->img_width);
    IOWR32(this->address, CAMERA_CAPTURE_HEIGHT, this->img_height);

    //Wait until Standby signal is 1. Its the way to ensure that the component
    //is not in reset or acquiring a signal.
    int counter = 10000000;
    while((!(IORD32(this->address, CAMERA_CAPTURE_STANDBY)))&&(counter>0))
    {
        //ugly way avoid software to get stuck
        counter--;
    }
    if (counter == 0) return CAMERA_NO_REPLY;

    //Now the component is in Standby (state1). Counters and full buffer
    //in image_capture component flags are automatically reset in hardware.
    //Reset the registers saving rising edges of buff0full and buff1full
    //in avalon camera, just in case the previous capture finished in
    //wrong way.
    IOWR32(this->address, CAMERA_BUFF0_FULL, 0);
    IOWR32(this->address, CAMERA_BUFF1_FULL, 0);

    //Start the capture (generate a pos flank in start_capture signal)
    IOWR32(this->address, CAMERA_START_CAPTURE, 1);
    IOWR32(this->address, CAMERA_START_CAPTURE, 0);

    return 0;
 }

 //capture_get_line()
 //
 // description:
 // waits until the current buffer is acquired with a new line.
 // it gives as return value the pointer of the line just acquired.
 // This buffer should be emptied before the next line is acquired otherwise
 // new data will overwrite the old line. The function checks just after
 // entering the flag of the line that is being acquired.
 //
 // parameters: line is a pointer to the buffer where image_capture component
 //             just acquired the current line. It is an output parameter passed
 //             byref using &.
 //
 // return: 0 success
 //         1 the line was already acquired when entering in the function
 //         (without any waiting).
 //         2 there was excesive waiting on the line.
 int Camera::capture_get_line(cpixel*& line)
 {
    int counter = 10000000;
    //if the camera is now saving in the buff0 (odd lines)
    if (this->current_buff_v == this->buff0_v)
    {
        //check if buff0 is full without waiting
        if ((IORD32(this->address, CAMERA_BUFF0_FULL)) == 1)
            return CAMERA_CAPTURE_GET_LINE_BUFFER_FULL_NO_WAIT;
        else
        {
            //wait for the line to be acquired
            while((!IORD32(this->address, CAMERA_BUFF0_FULL))&&(counter>0))
            {
                //ugly way avoid software to get stuck
                counter--;
            }
            if (counter == 0) return CAMERA_CAPTURE_GET_LINE_TIMEOUT;

            IOWR32(this->address, CAMERA_BUFF0_FULL, 0); //reset the flag
            this->current_buff_v = this->buff1_v;//change the acquisition buffer
            line = this->buff0_v; //return address of the current acquired line
            return 0;
        }
    }
    //if the camera is now saving in the buff1 (even lines)
    else
    {
        //check if buff1 is full without waiting
        if ((IORD32(this->address, CAMERA_BUFF1_FULL)) == 1)
            return CAMERA_CAPTURE_GET_LINE_BUFFER_FULL_NO_WAIT;
        else
        {
            //wait for the line to be acquired
            while((!IORD32(this->address, CAMERA_BUFF1_FULL))&&(counter>0))
            {
                //ugly way avoid software to get stuck
                counter--;
            }
            if (counter == 0) return CAMERA_CAPTURE_GET_LINE_TIMEOUT;

            IOWR32(this->address, CAMERA_BUFF1_FULL, 0); //reset the flag
            this->current_buff_v = this->buff0_v;//change the acquisition buffer
            line = this->buff1_v; //return address of the current acquired line
            return 0;
        }
    }
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
