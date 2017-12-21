#include "main.hpp"

void print_img_proc_config(ImageProcessing* img_proc) {
    printf("hue threshold Low: %u\n", ((unsigned int) img_proc->get_hue_th_L()));
    printf("hue threshold High: %u\n", ((unsigned int)  img_proc->get_hue_th_H()));
    printf("brightness threshold Low: %u\n", ((unsigned int) img_proc->get_brightness_th_L()));
    printf("brightness threshold High: %u\n", ((unsigned int) img_proc->get_brightness_th_H()));
    printf("saturation threshold Low: %u\n", ((unsigned int) img_proc->get_saturation_th_L()));
    printf("saturation threshold High: %u\n", ((unsigned int) img_proc->get_saturation_th_H()));
}

int main(int argc, char **argv) {
    void *virtual_base;
    int fd;
    void *img_proc_virtual_address;

    // Open the device file for accessing the physical memory
    if ((fd = open("/dev/mem", (O_RDWR | O_SYNC))) == -1) {
        printf("ERROR: could not open \"/dev/mem\"...\n");
        return(1);
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
    img_proc_virtual_address = (void*) ((uint8_t*)virtual_base + ((unsigned long) (AVALON_IMAGE_PROCESSING_0_BASE) & (unsigned long) (HW_REGS_MASK)));

    //Initialize camera
    ImageProcessing img_proc(img_proc_virtual_address);

    //Write the col_size value written by the user in the avalon_camera registers
    if ( argc == 7) {
        img_proc.set_hue_th_L(atoi(argv[1]));
        img_proc.set_hue_th_H(atoi(argv[2]));
        img_proc.set_brightness_th_L(atoi(argv[3]));
        img_proc.set_brightness_th_H(atoi(argv[4]));
        img_proc.set_saturation_th_L(atoi(argv[5]));
        img_proc.set_saturation_th_H(atoi(argv[6]));
        print_img_proc_config(&img_proc);
    }
    else if (argc == 2) {
        //show help message
        printf("HELP MESSAGE\n");
        printf("No arguments sets the default configuration\n");
        printf("Call with arguments to change the behaviour of the image processing\n");
        printf("FORMAT: img_processing_test hue_threshold_L hue_threshold_H brightness_threshold_L brightness_threshold_H saturation_threshold_L saturation_threshold_H\n");
        printf("EXAMPLE TO BINARIZE GREEN (120 degrees): ./img_processing_test 80 90 45 255 20 255\n");
        printf("EXAMPLE TO BINARIZE BLUE (240 degrees):  ./img_processing_test 165 175 45 255 20 255\n");
        printf("EXAMPLE TO BINARIZE RED (0/360 degrees): ./img_processing_test 250 5 45 255 20 255\n");
    }
    else {
        // Set default values.
        img_proc.set_default();
        print_img_proc_config(&img_proc);
    }

    // clean up the memory mapping and exit
    if (munmap( virtual_base, HW_REGS_SPAN ) != 0) {
        printf("ERROR: munmap() failed...\n");
        close(fd);
        return(1);
    }
    close(fd);
    return(0);
}
