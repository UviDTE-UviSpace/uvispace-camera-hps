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

void print_camera_config(Camera* cam)
{
    printf("width changed to: %u\n", ((unsigned int) cam->config_get_width()));
    printf("heigh changed to: %u\n", ((unsigned int)  cam->config_get_height()));
    printf("start_row changed to: %u\n", ((unsigned int) cam->config_get_start_row()));
    printf("start_col changed to: %u\n", ((unsigned int) cam->config_get_start_column()));
    printf("row_size changed to: %u\n", ((unsigned int)  cam->config_get_row_size()));
    printf("col_size changed to: %u\n", ((unsigned int)  cam->config_get_column_size()));
    printf("row_mode changed to: %u\n", ((unsigned int)  cam->config_get_row_mode()));
    printf("col_mode changed to: %u\n", ((unsigned int)  cam->config_get_column_mode()));
    printf("exposure changed to: %u\n", ((unsigned int)  cam->config_get_exposure()));
}

int main(int argc, char **argv) {
    void *virtual_base;
    int fd;
    void *camera_virtual_address;
    //void *dipsw_virtual_address;
    
    // Open the device file for accessing the physical memory.
    if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
        printf( "ERROR: could not open \"/dev/mem\"...\n" );
        return( 1 );
    }
    // Map the physical memory to the virtual address space. The base address
    // for the FPGA address map is stored in 'virtual_base'.
    virtual_base = mmap(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE),
                        MAP_SHARED, fd, HW_REGS_BASE);
    if( virtual_base == MAP_FAILED ) {
        printf( "ERROR: mmap() failed...\n" );
        close( fd );
        return( 1 );
    }
    // Virtual address of the camera registers.
    camera_virtual_address = (void*)((uint8_t*)virtual_base + ( ( unsigned long  )( AVALON_CAMERA_0_BASE ) & ( unsigned long)( HW_REGS_MASK ) ));
    
    //Initialize camera
    Camera cam(camera_virtual_address);
    
    //Write the col_size value written by the user in the avalon_camera registers
    if ( argc == 10)
    {
        cam.config_set_width(atoi(argv[1]));
        cam.config_set_height(atoi(argv[2]));
        cam.config_set_start_row(atoi(argv[3]));
        cam.config_set_start_column(atoi(argv[4]));
        cam.config_set_row_size(atoi(argv[5])) ;
        cam.config_set_column_size(atoi(argv[6])); 
        cam.config_set_row_mode(atoi(argv[7])); 
        cam.config_set_column_mode(atoi(argv[8]));
        cam.config_set_exposure(atoi(argv[9]));
        cam.update_config();
        print_camera_config(&cam);
    }
    else if (argc == 2)
    {
        //show help message
        printf("HELP MESSAGE\n");
        printf("No arguments sets the default configuration\n");
        printf("Call with arguments to change the behaviour of the camera\n");
        printf("FORMAT: camera_test width height start_row start_col row_size col_size row_mode col_mode exposure\n");
        printf("EXAMPLE: ./camera_test 640 480 0 0 1919 2559 17 17 1984\n");
    }
    else
    {
        // Set default values.
        cam.set_default_config();
        cam.update_config();
        print_camera_config(&cam);
    }
    
    // clean up the memory mapping and exit
    if( munmap( virtual_base, HW_REGS_SPAN ) != 0 ) {
        printf( "ERROR: munmap() failed...\n" );
        close( fd );
        return( 1 );
    }
    close( fd );
    return( 0 );
}


