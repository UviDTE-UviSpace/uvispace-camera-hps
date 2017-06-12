#include "camera_server.hpp"

camera_server::camera_server(int port) : abstract_server(port) {
}

std::string camera_server::process_request(std::string request) {
    if (request == "get_color") {
        return this->get_image(image_color);
    } if (request == "get_gray") {
        return this->get_image(image_grayscale);
    } else {
        return "unknown command\n";
    }
}

std::string camera_server::get_image(const int ic) {
    int fd;
    void *virtual_base_fpga;
    void *camera_virtual_address;
    void *virtual_base_hps_ocr;
    int fd_hps_ocr;

    // Open the device file for accessing the physical memory of FPGA
    if ((fd = open("/dev/mem", (O_RDWR | O_SYNC))) == -1) {
        std::cout << "ERROR: could not open \"/dev/mem\"..." << std::endl;
        return "";
    }
    // Map the physical memory to the virtual address space. The base address
    // for the FPGA address map is stored in 'virtual_base_fpga'.
    virtual_base_fpga = mmap(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, HW_REGS_BASE);
    if (virtual_base_fpga == MAP_FAILED) {
        std::cout << "ERROR: mmap() failed..." << std::endl;
        close( fd );
        return "";
    }
    // Virtual address of the camera registers.
    camera_virtual_address = (void*) ((uint8_t*) virtual_base_fpga + ((unsigned long) (AVALON_CAMERA_0_BASE) & (unsigned long) (HW_REGS_MASK)));

    // Map the physical HPS On-Chip RAM into the virtual address space
    // of this application to have access to it.
    virtual_base_hps_ocr = mmap(NULL, HPS_OCR_SPAM, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, HPS_OCR_BASE);
    if (virtual_base_hps_ocr == MAP_FAILED) {
        std::cout << "ERROR: mmap() failed..." << std::endl;
        close(fd);
        return "";
    }

    // Initialize camera
    avalon_camera cam(camera_virtual_address); //default configuration on creation

    // Capture one image
    cpixel image[IMAGE_HEIGHT][IMAGE_WIDTH];
    cam.capture_set_buffer(virtual_base_hps_ocr, (void*) HPS_OCR_BASE);
    int error = cam.capture_get_image(&image[0][0]);
    if (error == CAMERA_NO_REPLY) {
        std::cout << "Error starting the capture (component does not answer)" << std::endl;
        std::cout << "Image_capture is in reset or not connected to the bus" << std::endl;
        std::cout << "Return from the application..." << std::endl;
        return "";
    } else if (error == CAMERA_CAPTURE_GET_LINE_BUFFER_FULL_NO_WAIT) {
        std::cout << "Error, line already acquired when entering capture_get_line." << std::endl;
        std::cout << "Possible loss of data. The processor did too much in between two" << std::endl;
        std::cout << "calls to  capture_get_line. Try to reduce frame rate." << std::endl;
        std::cout << "Return from the application..." << std::endl;
        return "";
    } else if (error == CAMERA_CAPTURE_GET_LINE_TIMEOUT) {
        std::cout << "Error, Too much waiting for a new line. Looks like no video)" << std::endl;
        std::cout << "stream is there. Maybe the video stream is in reset state?" << std::endl;
        std::cout << "Return from the application..." << std::endl;
        return "";
    }

    // Print some pixels from image
    std::cout << "Image was successfully captured." << std::endl;

    std::string response = "";
    if (ic == image_color) {
        for (int i=0; i<IMAGE_HEIGHT; i++) {
            for (int j=0; j<IMAGE_WIDTH; j++) {
                response += image[i][j].R;
                response += image[i][j].G;
                response += image[i][j].B;
            }
        }
    } else if (ic == image_grayscale) {
        for (int i=0; i<IMAGE_HEIGHT; i++) {
            for (int j=0; j<IMAGE_WIDTH; j++) {
                response += image[i][j].Gray;
            }
        }
    }

    // Clean up the memory mapping and exit
    if (munmap(virtual_base_fpga, HW_REGS_SPAN) != 0) {
        std::cout << "ERROR: munmap() failed..." << std::endl;
        close(fd);
        return "";
    }
    if (munmap(virtual_base_hps_ocr, HPS_OCR_SPAM) != 0) {
        std::cout << "ERROR: munmap() failed..." << std::endl;
        close(fd);
        return "";
    }
    close(fd);
    return response;
}
