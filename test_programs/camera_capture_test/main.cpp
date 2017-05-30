// Required by hwlib.h 
#define soc_cv_av
// Standard libraries
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <inttypes.h>
#include <stdlib.h> // atoi() 
// DS-5 libraries. They are placed at <DS-5-path>/embedded/ip/altera/hps/altera_hps/hwlib/include
//#include "hwlib.h"
//#include "soc_cv_av/socal/socal.h"
//#include "soc_cv_av/socal/hps.h"
//#include "soc_cv_av/socal/alt_gpio.h"

#include "hps_0.h"
#include "avalon_camera.hpp"

//Constants to do mmap and get access to FPGA peripherals
#define HPS_FPGA_BRIDGE_BASE 0xC0000000
#define HW_REGS_BASE ( HPS_FPGA_BRIDGE_BASE )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

//Constants to mmap HPS On-Chip Ram (used to capture images)
#define HPS_OCR_BASE 0xFFFF0000
#define HPS_OCR_SPAM (64*1024) //64kB
#define HPS_OCR_MASK ( HPS_OCR_SPAM - 1 )

//Image properties
#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480

int main(int argc, char **argv) {
    
    int fd;
    void *virtual_base_fpga;
    void *camera_virtual_address;
    void *virtual_base_hps_ocr;
    int fd_hps_ocr;
    
    //------------GENERATE VIRTUAL ADDRESSES FOR HARDWARE ELEMENTS-----------//
    // Open the device file for accessing the physical memory of FPGA 
    if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
        printf( "ERROR: could not open \"/dev/mem\"...\n" );
        return( 1 );
    }
    // Map the physical memory to the virtual address space. The base address
    // for the FPGA address map is stored in 'virtual_base_fpga'.
    virtual_base_fpga = mmap(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE),
                        MAP_SHARED, fd, HW_REGS_BASE);
    if( virtual_base_fpga == MAP_FAILED ) {
        printf( "ERROR: mmap() failed...\n" );
        close( fd );
        return( 1 );
    }
    // Virtual address of the camera registers.
    camera_virtual_address = (void*)((uint8_t*)virtual_base_fpga + ( ( unsigned long  )( AVALON_CAMERA_0_BASE ) & ( unsigned long)( HW_REGS_MASK ) ));
    
    // Map the physical HPS On-Chip RAM into the virtual address space
    // of this application to have access to it.
    virtual_base_hps_ocr = mmap(NULL, HPS_OCR_SPAM, (PROT_READ | PROT_WRITE),
                        MAP_SHARED, fd, HPS_OCR_BASE);
    if( virtual_base_hps_ocr == MAP_FAILED ) {
        printf( "ERROR: mmap() failed...\n" );
        close( fd );
        return( 1 );
    }
    
    //----------CAPTURE IMAGE AND SHOW---------//
    //Initialize camera
    Camera cam(camera_virtual_address); //default configuration on creation
    cam.config_set_width(IMAGE_WIDTH); //change width
    cam.config_set_height(IMAGE_HEIGHT); //change height
    cam.config_update(); //update config changes so new config takes effect
    
    //Capture one image
    cpixel image[IMAGE_HEIGHT][IMAGE_WIDTH];
    cam.capture_set_buffer(virtual_base_hps_ocr, (void*)HPS_OCR_BASE);
    int error = cam.capture_get_image(&image[0][0]);
    if (error == 1)
    {
        printf("Error starting the capture (component does not answer)\n");
        printf("Image_capture is in reset or not connected to the bus\n");
        printf("Return from the application...\n");
        return 0;
    }
    else if (error == 2)
    {
        printf("Error, line already acquired when entering capture_get_line.\n");
        printf("Possible loss of data. The processor did too much in between two \n");
        printf("calls to  capture_get_line. Try to reduce frame rate.\n");
        printf("Return from the application...\n");
        return 0;
    }
    else if (error == 3)
    {
        printf("Error, Too much waiting for a new line. Looks like no video)\n");
        printf("stream is there. Maybe the video stream is in reset state?\n");
        printf("Return from the application...\n");
        return 0;
    }
    
    //Print some pixels from image
     printf("Image was successfully captured.\n");
    int i,j;
    for (i=0; i<IMAGE_HEIGHT; i++)
    {
        for (j=0; j<IMAGE_WIDTH; j++)
        {
            if ((i<2)&&(j<2))
            {
                printf("pix[%d,%d]:  R %2X  G %2X   B %2X   Gray %2X\n",
                i,
                j,
                image[i][j].R,
                image[i][j].G,
                image[i][j].B,
                image[i][j].Gray);
            }
        }
    }
            
    // clean up the memory mapping and exit
    if( munmap( virtual_base_fpga, HW_REGS_SPAN ) != 0 ) {
        printf( "ERROR: munmap() failed...\n" );
        close( fd );
        return( 1 );
    }
    if( munmap( virtual_base_hps_ocr, HPS_OCR_SPAM ) != 0 ) {
        printf( "ERROR: munmap() failed...\n" );
        close( fd );
        return( 1 );
    }
    close( fd );
    return( 0 );
}


