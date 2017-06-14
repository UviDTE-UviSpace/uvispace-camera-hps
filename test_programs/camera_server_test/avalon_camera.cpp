#include "avalon_camera.hpp"

avalon_camera::avalon_camera(void* virtual_address) {
    this->address = virtual_address;
    this->set_default_configuration();
    this->config_update();
}

uint16_t avalon_camera::get_width() {
    return IORD32(this->address, reg::config::width);
}

void avalon_camera::set_width(uint16_t val) {
    IOWR32(this->address, reg::config::width, val);
    this->img_width = val;
    return;
}

uint16_t avalon_camera::get_height() {
    return IORD32(this->address, reg::config::height);
}

void avalon_camera::set_height(uint16_t val) {
    IOWR32(this->address, reg::config::height, val);
    this->img_height = val;
    return;
}

uint16_t avalon_camera::get_row_start() {
    return IORD32(this->address, reg::config::row_start);
}

void avalon_camera::set_row_start(uint16_t val) {
    IOWR32(this->address, reg::config::row_start, val);
    return;
}

uint16_t avalon_camera::get_column_start() {
    return IORD32(this->address, reg::config::column_start);
}

void avalon_camera::set_column_start(uint16_t val) {
    IOWR32(this->address, reg::config::column_start, val);
    return;
}

uint16_t avalon_camera::get_row_end() {
    return IORD32(this->address, reg::config::row_end);
}

void avalon_camera::set_row_end(uint16_t val) {
    IOWR32(this->address, reg::config::row_end, val);
    return;
}

uint16_t avalon_camera::get_column_end() {
    return IORD32(this->address, reg::config::column_end);
}

void avalon_camera::set_column_end(uint16_t val) {
    IOWR32(this->address, reg::config::column_end, val);
    return;
}

uint16_t avalon_camera::get_row_mode() {
    return IORD32(this->address, reg::config::row_mode);
}

void avalon_camera::set_row_mode(uint16_t val) {
    IOWR32(this->address, reg::config::row_mode, val);
    return;
}

uint16_t avalon_camera::get_column_mode() {
    return IORD32(this->address, reg::config::column_mode);
}

void avalon_camera::set_column_mode(uint16_t val) {
    IOWR32(this->address, reg::config::column_mode, val);
    return;
}

uint16_t avalon_camera::get_exposure() {
    return IORD32(this->address, reg::config::exposure);
}

void avalon_camera::set_exposure(uint16_t val) {
    IOWR32(this->address, reg::config::exposure, val);
    return;
}

void avalon_camera::set_default_configuration() {
    this->set_width(CONFIG_WIDTH_DEFAULT);
    this->set_height(CONFIG_HEIGHT_DEFAULT);
    this->set_row_start(CONFIG_START_ROW_DEFAULT);
    this->set_column_start(CONFIG_START_COLUMN_DEFAULT);
    this->set_row_end(CONFIG_ROW_SIZE_DEFAULT);
    this->set_column_end(CONFIG_COLUMN_SIZE_DEFAULT);
    this->set_row_mode(CONFIG_ROW_MODE_DEFAULT);
    this->set_column_mode(CONFIG_COLUMN_MODE_DEFAULT);
    this->set_exposure(CONFIG_EXPOSURE_DEFAULT);
    return;
}

void avalon_camera::config_update() {
    this->reset();
    return;
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
void avalon_camera::capture_set_buffer(void* buffer_v, void* buffer_p) {
    this->buff_v = buffer_v;
    this->buff_p = buffer_p;
    return;
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
void avalon_camera::capture_image(cpixel* image) {
    cpixel* line;
    int line_size_B = this->img_width * LINES_PER_BUFF * sizeof(cpixel); //line size in Bytes

    //Start capture
    this->capture_start();

    //Capture lines
    for(int i=0; i<this->img_height/LINES_PER_BUFF; i++) { //for every line
        //wait till line is captured
        this->capture_get_line(line);

        //Copy the line from buffer for lines to big buffer in processor
        //memcpy is the fastest method to copy data (for dma a driver is needed)
        //void * memcpy ( void * destination, const void * source, size_t num );
        memcpy ((void*)&image[i*this->img_width*LINES_PER_BUFF], (void*)line, line_size_B );
    }
    return;
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

void avalon_camera::capture_start() {
    //generate the software addresses of buff0 and buff1
    this->buff0_v = (cpixel*) this->buff_v;
    this->buff1_v = (cpixel*)((uint8_t*)this->buff_v + sizeof(cpixel)*this->img_width*LINES_PER_BUFF);
    //First line will be saved in buff0
    this->current_buff_v = this->buff0_v;

    //generate physical addresses and save the into the avalon_camera
    //so image capture knows physical addresses of buff0 and buff1
    void* buff0_p = this->buff_p;
    void* buff1_p = (void*)((uint8_t*)this->buff_p + sizeof(cpixel)*this->img_width*LINES_PER_BUFF);
    IOWR32(this->address, reg::capture::buff0, buff0_p);
    IOWR32(this->address, reg::capture::buff1, buff1_p);

    //Indicate the image size to the capture_image component
    IOWR32(this->address, reg::capture::width, this->img_width*LINES_PER_BUFF);
    IOWR32(this->address, reg::capture::height, this->img_height/LINES_PER_BUFF);

    //Wait until Standby signal is 1. Its the way to ensure that the component
    //is not in reset or acquiring a signal.
    int counter = 10000000;
    while((!(IORD32(this->address, reg::capture::standby)))&&(counter>0)) {
            //ugly way avoid software to get stuck
            counter--;
    }
    if (counter == 0) {
        throw exception::camera_no_reply();
    }
    //Now the component is in Standby (state1). Counters and full buffer
    //in image_capture component flags are automatically reset in hardware.
    //Reset the registers saving rising edges of buff0full and buff1full
    //in avalon camera, just in case the previous capture finished in
    //wrong way.
    IOWR32(this->address, reg::capture::buff0_full, 0);
    IOWR32(this->address, reg::capture::buff1_full, 0);

    //Start the capture (generate a pos flank in start_capture signal)
    IOWR32(this->address, reg::capture::start, 1);
    IOWR32(this->address, reg::capture::start, 0);

    return;
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
void avalon_camera::capture_get_line(cpixel*& line) {
    int counter = 10000000;
    //if the camera is now saving in the buff0 (odd lines)
    if (this->current_buff_v == this->buff0_v) {
        //check if buff0 is full without waiting
        if ((IORD32(this->address, reg::capture::buff0_full)) == 1) {
            // return CAMERA_CAPTURE_GET_LINE_BUFFER_FULL_NO_WAIT;
            throw exception::capture_buffer_full();
        } else {
            //wait for the line to be acquired
            while((!IORD32(this->address, reg::capture::buff0_full))&&(counter>0)) {
                //ugly way avoid software to get stuck
                counter--;
            }
            if (counter == 0) {
                // return CAMERA_CAPTURE_GET_LINE_TIMEOUT;
                throw exception::capture_timeout();
            }
            IOWR32(this->address, reg::capture::buff0_full, 0); //reset the flag
            this->current_buff_v = this->buff1_v; //change the acquisition buffer
            line = this->buff0_v; //return address of the current acquired line
            return;
        }
    } else { //if the camera is now saving in the buff1 (even lines)
        //check if buff1 is full without waiting
        if ((IORD32(this->address, reg::capture::buff1_full)) == 1) {
            // return CAMERA_CAPTURE_GET_LINE_BUFFER_FULL_NO_WAIT;
            throw exception::capture_buffer_full();
        } else {
            //wait for the line to be acquired
            while((!IORD32(this->address, reg::capture::buff1_full))&&(counter>0)) {
                //ugly way avoid software to get stuck
                counter--;
            }
            if (counter == 0) {
                // return CAMERA_CAPTURE_GET_LINE_TIMEOUT;
                throw exception::capture_timeout();
            }
            IOWR32(this->address, reg::capture::buff1_full, 0); //reset the flag
            this->current_buff_v = this->buff0_v; //change the acquisition buffer
            line = this->buff1_v; //return address of the current acquired line
            return;
        }
    }
}

//reset
int avalon_camera::reset() {
    //soft_reset is active low so when 0 the camera is reset
    IOWR32(this->address, reg::reset::soft, 0); //reset
    IOWR32(this->address, reg::reset::soft, 1); //remove reset
    return 0; //return 0 on success
}
