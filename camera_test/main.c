// Required by hwlib.h 
#define soc_cv_av
// Standard libraries
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <inttypes.h>
// DS-5 libraries. They are placed at <DS-5-path>/embedded/ip/altera/hps/altera_hps/hwlib/include
//#include "hwlib.h"
//#include "soc_cv_av/socal/socal.h"
//#include "soc_cv_av/socal/hps.h"
//#include "soc_cv_av/socal/alt_gpio.h"
// Local project header.
#include "hps_0.h"
#include "avalon_camera.h"

#define HPS_FPGA_BRIDGE_BASE 0xC0000000
#define HW_REGS_BASE ( HPS_FPGA_BRIDGE_BASE )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

int main(int argc, char **argv) {
    void *virtual_base;
    int fd;
    void *camera_virtual_address;
    void *dipsw_virtual_address;
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
    camera_virtual_address = virtual_base + ( ( unsigned long  )( AVALON_CAMERA_BASE ) & ( unsigned long)( HW_REGS_MASK ) );
    // Virtual address of the board switches.
    dipsw_virtual_address = virtual_base + ( ( unsigned long  )( DIPSW_PIO_BASE ) & ( unsigned long)( HW_REGS_MASK ) );
    // Write some random value to the exposure register.
    printf("exposure: %u\n", ((unsigned int) IORD_CAMERA_EXPOSURE(camera_virtual_address)));
    (IOWR_CAMERA_EXPOSURE(camera_virtual_address, 0x2300));
    // The switches value is printed every second.
    while(1){
        printf("Switches: %u\n", (*((unsigned int *) dipsw_virtual_address)));
        printf("exposure: %u\n", ((unsigned int) IORD_CAMERA_EXPOSURE(camera_virtual_address)));
        // wait 1s.
        usleep( 1000*1000 );
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
