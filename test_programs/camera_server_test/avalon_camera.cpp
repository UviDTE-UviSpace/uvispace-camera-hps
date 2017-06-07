#include "avalon_camera.hpp"

//class constructor (called when object is created)
avalon_camera::avalon_camera(void* virtual_address) {
    this->address = virtual_address;
    this->config_set_default();
    this->config_update();
}

//methods to set the camera configuration
int avalon_camera::config_set_width(uint16_t val) {
    IOWR32(this->address, ADDR_WIDTH, val);
    this->img_width = val;
    return 0;
}

int avalon_camera::config_set_height(uint16_t val) {
    IOWR32(this->address, ADDR_HEIGHT, val);
    this->img_height = val;
    return 0;
}

int avalon_camera::config_set_start_row(uint16_t val) {
    IOWR32(this->address, ADDR_START_ROW, val);
    return 0;
}

int avalon_camera::config_set_start_column(uint16_t val) {
    IOWR32(this->address, ADDR_START_COLUMN, val);
    return 0;
}

int avalon_camera::config_set_row_size(uint16_t val) {
    IOWR32(this->address, ADDR_ROW_SIZE, val);
    return 0;
}

int avalon_camera::config_set_column_size(uint16_t val) {
    IOWR32(this->address, ADDR_COLUMN_SIZE, val);
    return 0;
}

int avalon_camera::config_set_row_mode(uint16_t val) {
    IOWR32(this->address, ADDR_ROW_MODE, val);
    return 0;
}

int avalon_camera::config_set_column_mode(uint16_t val) {
    IOWR32(this->address, ADDR_COLUMN_MODE, val);
    return 0;
}

int avalon_camera::config_set_exposure(uint16_t val) {
    IOWR32(this->address, ADDR_EXPOSURE, val);
    return 0;
}

int avalon_camera::config_set_default(void) {
    this->config_set_width(CONFIG_WIDTH_DEFAULT);
    this->config_set_height(CONFIG_HEIGHT_DEFAULT);
    this->config_set_start_row(CONFIG_START_ROW_DEFAULT);
    this->config_set_start_column(CONFIG_START_COLUMN_DEFAULT);
    this->config_set_row_size(CONFIG_ROW_SIZE_DEFAULT);
    this->config_set_column_size(CONFIG_COLUMN_SIZE_DEFAULT);
    this->config_set_row_mode(CONFIG_ROW_MODE_DEFAULT);
    this->config_set_column_mode(CONFIG_COLUMN_MODE_DEFAULT);
    this->config_set_exposure(CONFIG_EXPOSURE_DEFAULT);
    return 0;
}

int avalon_camera::config_update(void) {
    //this function is equal to reset now but in the future
    //could not be. So use this to update the camera config.
    this->reset();
    return 0;
}

//Methods to get the camera configuration
uint16_t avalon_camera::config_get_width(void) {
    return IORD32(this->address, ADDR_WIDTH);
}

uint16_t avalon_camera::config_get_height(void) {
    return IORD32(this->address, ADDR_HEIGHT);
}

uint16_t avalon_camera::config_get_start_row(void) {
    return IORD32(this->address, ADDR_START_ROW);
}

uint16_t avalon_camera::config_get_start_column(void) {
    return IORD32(this->address, ADDR_START_COLUMN);
}

uint16_t avalon_camera::config_get_row_size(void) {
    return IORD32(this->address, ADDR_ROW_SIZE);
}

uint16_t avalon_camera::config_get_column_size(void) {
    return IORD32(this->address, ADDR_COLUMN_SIZE);
}

uint16_t avalon_camera::config_get_row_mode(void) {
    return IORD32(this->address, ADDR_ROW_MODE);
}

uint16_t avalon_camera::config_get_column_mode(void) {
    return IORD32(this->address, ADDR_COLUMN_MODE);
}

uint16_t avalon_camera::config_get_exposure(void) {
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
int avalon_camera::capture_set_buffer(void* buffer_v, void* buffer_p) {
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
int avalon_camera::capture_get_image(cpixel* image) {
    int error;
    int i;
    cpixel* line;
    int line_size_B = this->img_width * LINES_PER_BUFF * sizeof(cpixel); //line size in Bytes

    //Start capture
    error = this->capture_start();
    if (error == CAMERA_NO_REPLY) {
        return CAMERA_NO_REPLY;
    }

    //Capture lines
    for(i=0; i<this->img_height/LINES_PER_BUFF; i++) { //for every line
        //wait till line is captured
        error = this->capture_get_line(line);
        if (error == CAMERA_CAPTURE_GET_LINE_BUFFER_FULL_NO_WAIT) {
            std::cout << "Error in line " << i << std::endl;
            return CAMERA_CAPTURE_GET_LINE_BUFFER_FULL_NO_WAIT;
        }
        else if (error == CAMERA_CAPTURE_GET_LINE_TIMEOUT) {
            return CAMERA_CAPTURE_GET_LINE_TIMEOUT;
        }

        //Copy the line from buffer for lines to big buffer in processor
        //memcpy is the fastest method to copy data (for dma a driver is needed)
        //void * memcpy ( void * destination, const void * source, size_t num );
        memcpy ((void*)&image[i*this->img_width*LINES_PER_BUFF], (void*)line, line_size_B );
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

int avalon_camera::capture_start(void) {
    //generate the software addresses of buff0 and buff1
    this->buff0_v = (cpixel*) this->buff_v;
    this->buff1_v = (cpixel*)((uint8_t*)this->buff_v + sizeof(cpixel)*this->img_width*LINES_PER_BUFF);
    //First line will be saved in buff0
    this->current_buff_v = this->buff0_v;

    //generate physical addresses and save the into the avalon_camera
    //so image capture knows physical addresses of buff0 and buff1
    void* buff0_p = this->buff_p;
    void* buff1_p = (void*)((uint8_t*)this->buff_p + sizeof(cpixel)*this->img_width*LINES_PER_BUFF);
    IOWR32(this->address, CAMERA_BUFF0, buff0_p);
    IOWR32(this->address, CAMERA_BUFF1, buff1_p);

    //Indicate the image size to the capture_image component
    IOWR32(this->address, CAMERA_CAPTURE_WIDTH, this->img_width*LINES_PER_BUFF);
    IOWR32(this->address, CAMERA_CAPTURE_HEIGHT, this->img_height/LINES_PER_BUFF);

    //Wait until Standby signal is 1. Its the way to ensure that the component
    //is not in reset or acquiring a signal.
    int counter = 10000000;
    while((!(IORD32(this->address, CAMERA_CAPTURE_STANDBY)))&&(counter>0)) {
            //ugly way avoid software to get stuck
            counter--;
    }
    if (counter == 0) {
        return CAMERA_NO_REPLY;
    }
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
int avalon_camera::capture_get_line(cpixel*& line) {
    int counter = 10000000;
    //if the camera is now saving in the buff0 (odd lines)
    if (this->current_buff_v == this->buff0_v) {
        //check if buff0 is full without waiting
        if ((IORD32(this->address, CAMERA_BUFF0_FULL)) == 1) {
            return CAMERA_CAPTURE_GET_LINE_BUFFER_FULL_NO_WAIT;
        } else {
            //wait for the line to be acquired
            while((!IORD32(this->address, CAMERA_BUFF0_FULL))&&(counter>0)) {
                //ugly way avoid software to get stuck
                counter--;
            }
            if (counter == 0) {
                return CAMERA_CAPTURE_GET_LINE_TIMEOUT;
            }
            IOWR32(this->address, CAMERA_BUFF0_FULL, 0); //reset the flag
            this->current_buff_v = this->buff1_v; //change the acquisition buffer
            line = this->buff0_v; //return address of the current acquired line
            return 0;
        }
    } else { //if the camera is now saving in the buff1 (even lines)
        //check if buff1 is full without waiting
        if ((IORD32(this->address, CAMERA_BUFF1_FULL)) == 1) {
            return CAMERA_CAPTURE_GET_LINE_BUFFER_FULL_NO_WAIT;
        } else {
            //wait for the line to be acquired
            while((!IORD32(this->address, CAMERA_BUFF1_FULL))&&(counter>0)) {
                //ugly way avoid software to get stuck
                counter--;
            }
            if (counter == 0) {
                return CAMERA_CAPTURE_GET_LINE_TIMEOUT;
            }
            IOWR32(this->address, CAMERA_BUFF1_FULL, 0); //reset the flag
            this->current_buff_v = this->buff0_v; //change the acquisition buffer
            line = this->buff1_v; //return address of the current acquired line
            return 0;
        }
    }
}

//reset
int avalon_camera::reset(void) {
    //soft_reset is active low so when 0 the camera is reset
    IOWR32(this->address, CAMERA_SOFT_RESET, 0); //reset
    IOWR32(this->address, CAMERA_SOFT_RESET, 1); //remove reset
    return 0; //return 0 on success
}
