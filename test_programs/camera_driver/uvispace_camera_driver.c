#include <linux/init.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/types.h>
#include <asm/uaccess.h>
#include "hps_0.h"

#define DEVICE_NAME "uvispace_camera"
#define CLASS_NAME "uvispace_camera"

#define IMAGE_HEIGHT 480
#define IMAGE_WIDTH 640

#define HPS_FPGA_BRIDGE_BASE 0xC0000000
#define PIXEL_SIZE sizeof(u8) * 4
#define IMAGE_MEM (PIXEL_SIZE * IMAGE_HEIGHT * IMAGE_WIDTH)
#define LINES_PER_BUFF IMAGE_HEIGHT

// Registers to control image_capture component
#define CAMERA_START_CAPTURE           0x00
#define CAMERA_CAPTURE_WIDTH           0x04
#define CAMERA_CAPTURE_HEIGHT          0x08
#define CAMERA_BUFF0                   0x0c
#define CAMERA_BUFF1                   0x10
#define CAMERA_BUFF0_FULL              0x14
#define CAMERA_BUFF1_FULL              0x18
#define CAMERA_CAPTURE_STANDBY         0x1c
// Registers to control camera_config component
#define ADDR_WIDTH                     0x24
#define ADDR_HEIGHT                    0x28
#define ADDR_START_ROW                 0x2c
#define ADDR_START_COLUMN              0x30
#define ADDR_ROW_SIZE                  0x34
#define ADDR_COLUMN_SIZE               0x38
#define ADDR_ROW_MODE                  0x3c
#define ADDR_COLUMN_MODE               0x40
#define ADDR_EXPOSURE                  0x44
// soft_reset signal
#define CAMERA_SOFT_RESET              0x7c

// Camera errors
#define ERROR_CAMERA_NO_REPLY 1

// Module information
MODULE_AUTHOR("Department of Electronic Technology, University of Vigo");
MODULE_DESCRIPTION("uvispace_camera: character device driver for uvispace camera");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

// Device driver variables
static int majorNumber;
static struct class* class = NULL;
static struct device* device = NULL;

// Camera variables
static void* address_virtual_camera;

static void* address_virtual_buffer;
static dma_addr_t address_physical_buffer;

// Prototype functions for the character driver
static int camera_open(struct inode *, struct file *);
static int camera_release(struct inode *, struct file *);
static ssize_t camera_read(struct file *, char *, size_t, loff_t *);

static struct file_operations fops = {
    .open = camera_open,
    .read = camera_read,
    .release = camera_release,
};

static int __init camera_driver_init(void) {
    printk(KERN_INFO DEVICE_NAME": Init\n");
    // Dynamically allocate a major number for the device
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        printk(KERN_ALERT DEVICE_NAME": Failed to register a major number\n");
    }
    // Register the device class
    class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(class)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT DEVICE_NAME": Failed to register device class\n");
        return PTR_ERR(class);
    }
    // Register the device driver
    device = device_create(class, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(device)) {
        class_destroy(class);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT DEVICE_NAME": Failed to create the device\n");
        return PTR_ERR(device);
    }
    return 0;
}

static void __exit camera_driver_exit(void) {
    device_destroy(class, MKDEV(majorNumber, 0));
    class_unregister(class);
    class_destroy(class);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO DEVICE_NAME": Exit\n");
}

static int camera_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO DEVICE_NAME": Open\n");

    // Ioremap FPGA memory //
    // To ioremap the memory in the FPGA so we can access from kernel space
    address_virtual_camera = ioremap(HPS_FPGA_BRIDGE_BASE + AVALON_CAMERA_0_BASE, AVALON_CAMERA_0_SPAN);
    if (address_virtual_camera == NULL) {
        printk(KERN_INFO DEVICE_NAME": Error doing FPGA camera ioremap\n");
        return -1;
    }

    // Allocate uncached buffer
    // The dma_alloc_coherent() function allocates non-cached physically
    // contiguous memory. Accesses to the memory by the CPU are the same
    // as a cache miss when the cache is used. The CPU does not have to
    // invalidate or flush the cache which can be time consuming.
    address_virtual_buffer = dma_alloc_coherent(
        NULL,
        IMAGE_MEM,
        &address_physical_buffer, //address to use from DMAC
        GFP_KERNEL);

    if (address_virtual_buffer == NULL) {
        printk(KERN_INFO DEVICE_NAME": allocation of non-cached buffer failed\n");
        return -1;
    }

    return 0;
}

int camera_capture_image(char* user_read_buffer) {
    int counter;
    int error_count;
    // Save physical addresses into the avalon_camera
    iowrite32(address_physical_buffer, address_virtual_camera + CAMERA_BUFF0);

    // Indicate the image size to the capture_image component
    iowrite32(IMAGE_WIDTH * LINES_PER_BUFF, address_virtual_camera + CAMERA_CAPTURE_WIDTH);
    iowrite32(IMAGE_HEIGHT / LINES_PER_BUFF, address_virtual_camera + CAMERA_CAPTURE_HEIGHT);

    // Wait until Standby signal is 1. Its the way to ensure that the component
    // is not in reset or acquiring a signal.
    counter = 10000000;
    while((!(ioread32(address_virtual_camera + CAMERA_CAPTURE_STANDBY))) && (counter>0)) {
        // Ugly way avoid software to get stuck
        counter--;
    }
    if (counter == 0) {
        printk(KERN_INFO DEVICE_NAME": Camera no reply\n");
        return ERROR_CAMERA_NO_REPLY;
    }

    // Start the capture (generate a pos flank in start_capture signal)
    iowrite32(1, address_virtual_camera + CAMERA_START_CAPTURE);
    iowrite32(0, address_virtual_camera + CAMERA_START_CAPTURE);

    // Wait for the image to be acquired
    while (!ioread32(address_virtual_camera + CAMERA_CAPTURE_STANDBY)) {}

    // Copy the image from buffer camera buffer to user buffer
    error_count = copy_to_user(user_read_buffer, address_virtual_buffer, IMAGE_MEM);
    if (error_count != 0) {
        printk(KERN_INFO DEVICE_NAME": Failed to send %d characters to the user in read function\n", error_count);
        return -EFAULT;  // Failed -- return a bad address message (i.e. -14)
    }
    return 0;
}

static ssize_t camera_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    int error;
    printk(KERN_INFO DEVICE_NAME": Read\n");
    error = camera_capture_image(buffer);
    if (error != 0) {
        printk(KERN_INFO DEVICE_NAME": Read failure\n");
        return 0;
    }
    return IMAGE_MEM;
}

static int camera_release(struct inode *inodep, struct file *filep) {
    dma_free_coherent(NULL, IMAGE_MEM, address_virtual_buffer, address_physical_buffer);
    iounmap(address_virtual_camera);
    printk(KERN_INFO DEVICE_NAME": Release\n");
    return 0;
}

module_init(camera_driver_init);
module_exit(camera_driver_exit);
