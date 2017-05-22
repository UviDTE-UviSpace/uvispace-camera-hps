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
                        MAP_SHARED, fd, HPS_OCR_SPAM);
    if( virtual_base_hps_ocr == MAP_FAILED ) {
        printf( "ERROR: mmap() failed...\n" );
        close( fd );
        return( 1 );
    }
    
    //----------WAIT FOR CODES FROM INTERNET AND RESPOND ACCORDINGLY---------//
    //Initialize camera
    Camera cam(camera_virtual_address);
    cam.config_set_default();
    cam.config_update();
    
    //Code showing how to capture one image
    cpixel* image_line;
    cam.capture_start(virtual_base_hps_ocr, (void*)HPS_OCR_BASE);
    int i,j;
    for(i=0; i<cam.img_height; i++) //for every line
    {
        image_line = cam.capture_get_line(); //wait till line is captured
        for(j=0; j<cam.img_width; j++) //for every pixel in the line
        {
            //you have access to each pixel in the line through:
            //image_line[j] 32-bit word: MSB[Gray, B, G, R]LSB
            
            //or individual access to each 8-bit component:
            //image_line[j].R 
            //image_line[j].G
            //image_line[j].B
            //image_line[j].Gray
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


