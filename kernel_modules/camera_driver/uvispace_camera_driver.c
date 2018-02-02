#include <linux/init.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/fs.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/types.h>
#include <asm/uaccess.h>

#include "hps_0.h"
#include "avalon_image_writer_regs.h"

#define HPS_FPGA_BRIDGE_BASE 0xC0000000

#define DRIVER_NAME "uvispace_camera"

#define CLASS_NAME "uvispace_camera"

#define DEV_NAME_RGBG "uvispace_camera_rgbg"
#define DEV_NAME_GRAY "uvispace_camera_gray"
#define DEV_NAME_BIN "uvispace_camera_bin"

//Minor numbers (used to identify each device because they share file operations)
#define MINOR_RGBG 0
#define MINOR_GRAY 1
#define MINOR_BIN  2

// Img writer mode.
// SINGLE_SHOT when reading it waits until a frame starts, captures it and comes back to iddle.
// CONTINUOUS starts continuous capture in 2 buffers so there is always an image ready when reading.
// Use SINGLE_SHOT for getting a picture and CONTINUOUS for getting a video. If CONTINUOUS is
// used to get a picture, if open and read are executed too fast the first image may not be
// acquired yet. If SINGLE_SHOT is used for video it will just go slower.
#define SINGLE_SHOT 0
#define CONTINUOUS  1

// Default image dimensions
#define DEFAULT_IMAGE_HEIGHT 480
#define DEFAULT_IMAGE_WIDTH  640

// Camera errors
#define ERROR_CAMERA_NO_REPLY 1

 //SDRAMC beginning address
#define SDRAMC_REGS 0xFFC20000
//SDRAMC Span 128kB
#define SDRAMC_REGS_SPAN 0x20000 //128kB
//Offset of FPGA-to-SDRAMC ports reset register from the beginning of SDRAMC
#define FPGAPORTRST 0x5080

// Module information
MODULE_AUTHOR("Department of Electronic Technology, University of Vigo");
MODULE_DESCRIPTION("uvispace_camera: character device driver for uvispace camera");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

// Device driver variables
static int majorNumber;
static struct class* class = NULL;
static struct device* device_rgbg = NULL;
static struct device* device_gray = NULL;
static struct device* device_bin = NULL;

// Image writer variables (one for each image_writer)
// 0 is RGBGray, 1 is Gray and 2 is Bin (same as minor numbers)
static void* address_virtual_image_writer[3];
static int is_open[3];
static void* address_virtual_buffer0[3];
static dma_addr_t address_physical_buffer0[3];
static void* address_virtual_buffer1[3];
static dma_addr_t address_physical_buffer1[3];
static size_t image_memory_size[3];
static int32_t last_image_number[3];

// Function prototypes
static int camera_open(struct inode *, struct file *);
static int camera_release(struct inode *, struct file *);
static ssize_t camera_read(struct file *, char *, size_t, loff_t *);
int camera_capture_image(char* user_read_buffer);

static struct file_operations fops = {
    .open = camera_open,
    .read = camera_read,
    .release = camera_release,
};

// Sysfs variables
static int image_height = DEFAULT_IMAGE_HEIGHT;
static int image_width = DEFAULT_IMAGE_WIDTH;
static int image_writer_mode = CONTINUOUS;

static ssize_t image_height_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
  return sprintf(buf, "%d\n", image_height);
}

static ssize_t image_height_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
  sscanf(buf, "%du", &image_height);
  return count;
}
static ssize_t image_width_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
  return sprintf(buf, "%d\n", image_width);
}

static ssize_t image_width_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
  sscanf(buf, "%du", &image_width);
  return count;
}

static ssize_t image_writer_mode_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
  return sprintf(buf, "%d\n", image_writer_mode);
}

static ssize_t image_writer_mode_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
  sscanf(buf, "%du", &image_writer_mode);
  return count;
}

static struct kobj_attribute image_height_attribute = __ATTR(image_height, 0660, image_height_show, image_height_store);
static struct kobj_attribute image_width_attribute = __ATTR(image_width, 0660, image_width_show, image_width_store);
static struct kobj_attribute image_writer_mode_attribute = __ATTR(image_writer_mode, 0660, image_writer_mode_show, image_writer_mode_store);

static struct attribute *uvispace_camera_attributes[] = {
      &image_height_attribute.attr,
      &image_width_attribute.attr,
      &image_writer_mode_attribute.attr,
      NULL,
};

static struct attribute_group attribute_group = {
      .name  = "attributes",
      .attrs = uvispace_camera_attributes,    ///< The attributes array defined just above
};

static struct kobject *uvispace_camera_kobj;


//------INIT AND EXIT FUNCTIONS-----//
static int __init camera_driver_init(void) {
    int result;
    int i;
    void* SDRAMC_virtual_address;

    printk(KERN_INFO DRIVER_NAME": Init\n");
    // Dynamically allocate a major number for the device
    majorNumber = register_chrdev(0, DRIVER_NAME, &fops);
    if (majorNumber < 0) {
        printk(KERN_ALERT DRIVER_NAME": Failed to register a major number\n");
        return 1;
    }
    // Register the device class
    class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(class)) {
        printk(KERN_ALERT DRIVER_NAME": Failed to register device class\n");
        goto error_class_create;
    }
    // Register the RGBG device
    device_rgbg = device_create(class, NULL, MKDEV(majorNumber, MINOR_RGBG), NULL, DEV_NAME_RGBG);
    if (IS_ERR(device_rgbg)) {
        printk(KERN_ALERT DRIVER_NAME": Failed to create the device RGBG\n");
        goto error_create_rgbg;
    }
    // Register the GRAY device
    device_gray = device_create(class, NULL, MKDEV(majorNumber, MINOR_GRAY), NULL, DEV_NAME_GRAY);
    if (IS_ERR(device_gray)) {
        printk(KERN_ALERT DRIVER_NAME": Failed to create the device GRAY\n");
        goto error_create_gray;
    }
    // Register the BIN device
    device_bin = device_create(class, NULL, MKDEV(majorNumber, MINOR_BIN), NULL, DEV_NAME_BIN);
    if (IS_ERR(device_bin)) {
        printk(KERN_ALERT DRIVER_NAME": Failed to create the device BIN\n");
        goto error_create_bin;
    }

    // Export sysfs variables
    // kernel_kobj points to /sys/kernel
    uvispace_camera_kobj = kobject_create_and_add(DRIVER_NAME, kernel_kobj->parent);
    if (!uvispace_camera_kobj) {
        printk(KERN_INFO DRIVER_NAME": Failed to create kobject mapping\n");
        goto error_create_kobj;
    }
    // add the attributes to /sys/uvispace_camera/attributes
    result = sysfs_create_group(uvispace_camera_kobj, &attribute_group);
    if (result) {
        printk(KERN_INFO DRIVER_NAME": Failed to create sysfs group\n");
        goto error_create_kobj;
    }

    // Reset the variables that flag if a device is already Open
    for (i=0; i<3; i++) is_open[i] = 0;

    //Remove FPGA-to-SDRAMC ports from reset so FPGA can access SDRAM from them
    SDRAMC_virtual_address = ioremap(SDRAMC_REGS, SDRAMC_REGS_SPAN);
    if (SDRAMC_virtual_address == NULL)
    {
      printk(KERN_INFO "DMA LKM: error doing SDRAMC ioremap\n");
      goto error_create_kobj;
    }
    *((unsigned int *)(SDRAMC_virtual_address + FPGAPORTRST)) = 0xFFFF;

    return 0;

    //Undo what it was done in case of error
error_create_kobj:
    device_destroy(class, MKDEV(majorNumber, MINOR_BIN));
error_create_bin:
    device_destroy(class, MKDEV(majorNumber, MINOR_GRAY));
error_create_gray:
    device_destroy(class, MKDEV(majorNumber, MINOR_RGBG));
error_create_rgbg:
    class_unregister(class);
    class_destroy(class);
error_class_create:
    unregister_chrdev(majorNumber, DRIVER_NAME);
    return -1;
}

static void __exit camera_driver_exit(void) {
    device_destroy(class, MKDEV(majorNumber, MINOR_BIN));
    device_destroy(class, MKDEV(majorNumber, MINOR_GRAY));
    device_destroy(class, MKDEV(majorNumber, MINOR_RGBG));
    class_unregister(class);
    class_destroy(class);
    unregister_chrdev(majorNumber, DRIVER_NAME);
    kobject_put(uvispace_camera_kobj);
    printk(KERN_INFO DRIVER_NAME": Exit\n");
}

//-----SMALL API TO CONTROL THE CAMERA-----//
int camera_setup(int n){
    // Save the mode (SINGLE_SHOT or CONTINUOUS)
    iowrite32(image_writer_mode, address_virtual_image_writer[n] + CAPTURE_MODE);

    // Save physical addresses into the avalon_camera
    iowrite32(address_physical_buffer0[n], address_virtual_image_writer[n] + CAPTURE_BUFF0);
    iowrite32(address_physical_buffer1[n], address_virtual_image_writer[n] + CAPTURE_BUFF1);

    // Choose buffer 0 to be used in SINGLE_SHOT
    iowrite32(0, address_virtual_image_writer[n] + CAPTURE_BUFFER_SELECT);

    // Choose to use 2 alternating buffers in CONTINUOUS mode
    iowrite32(1, address_virtual_image_writer[n] + CONT_DOUBLE_BUFF);

    // Set up downsampling as 1 to get the whole image
    iowrite32(1, address_virtual_image_writer[n] + CAPTURE_DOWNSAMPLING);

    return 0;
}

int camera_start_capture(int n) {
    int counter;

    //Stop the capture (to ensure a known state)
    iowrite32(0, address_virtual_image_writer[n] + START_CAPTURE);

    // Wait until Standby signal is 1. Its the way to ensure that the component
    // is not in reset or acquiring a signal.
    counter = 10000000;
    while((!(ioread32(address_virtual_image_writer[n] + CAPTURE_STANDBY))) && (counter>0)) {
        // Ugly way avoid software to get stuck
        counter--;
    }
    if (counter == 0) {
        printk(KERN_INFO DRIVER_NAME": Camera no reply\n");
        return ERROR_CAMERA_NO_REPLY;
    }

    // Start the capture
    iowrite32(1, address_virtual_image_writer[n] + START_CAPTURE);

    return 0;
}

// Stop the capture
int camera_stop_capture(int n) {
    // Stop the capture
    iowrite32(0, address_virtual_image_writer[n] + START_CAPTURE);
    return 0;
}


int camera_get_image(int n, char* user_read_buffer) {
    int error;
    int last_buffer;
    void* address_virtual_buffer;

    if (image_writer_mode == SINGLE_SHOT)
    {
        //Start capture
        error = camera_start_capture(n);
        if (error != 0) {
            printk(KERN_INFO DRIVER_NAME": Start capture failure\n");
            return error;
        }

        // Wait for the image to be acquired
        while (!ioread32(address_virtual_image_writer[n] + CAPTURE_STANDBY)) {}

        //In SINGLE_SHOT image is always saved in buffer 0
        address_virtual_buffer = address_virtual_buffer0[n];
    }
    else // (image_writer_mode == CONTINUOUS)
    {
        //In case the software applicattions ask for images faster than the hardware can provide
        //block the execution here until a new image is available
        while (ioread32(address_virtual_image_writer[n] + CAPTURE_IMAGE_COUNTER)
        == last_image_number[n]){};

        last_image_number[n] = ioread32(address_virtual_image_writer[n] + CAPTURE_IMAGE_COUNTER);

        //printk(KERN_INFO DRIVER_NAME": Image number %d\n", (int) last_image_number[n]);

        //Capture already started so just check where the last image was saved
        last_buffer = ioread32(address_virtual_image_writer[n] + LAST_BUFFER_CAPTURED);
        if (last_buffer == 0)
            address_virtual_buffer = address_virtual_buffer0[n];
        else
            address_virtual_buffer = address_virtual_buffer1[n];
    }

    // Copy the image from buffer camera buffer to user buffer
    error = copy_to_user(user_read_buffer, address_virtual_buffer, image_memory_size[n]);

    if (error != 0) {
        printk(KERN_INFO DRIVER_NAME": Failed to send %d characters to the user in read function\n", error);
        return -EFAULT;  // Failed -- return a bad address message (i.e. -14)
    }
    return 0;
}


//-----CHAR DEVICE DRIVER SPECIFIC FUNCTIONS-----//
static int camera_open(struct inode *inodep, struct file *filep) {
    int error;
    int image_writer_base;
    int image_writer_span;
    int pixel_size;

    //Findout which device is being open using the minor numbers
    int dev_number = iminor(filep->f_path.dentry->d_inode);

    // Establish image_writer and pixel_size based on image_type
    if (dev_number == MINOR_RGBG) {
        printk(KERN_INFO DRIVER_NAME": Open RGBG\n");
        image_writer_base = AVALON_IMG_WRITER_RGBGRAY_BASE;
        image_writer_span = AVALON_IMG_WRITER_RGBGRAY_SPAN;
        pixel_size = sizeof(u8) * 4;
    } else if (dev_number == MINOR_GRAY) {
        printk(KERN_INFO DRIVER_NAME": Open GRAY\n");
        image_writer_base = AVALON_IMG_WRITER_GRAY_BASE;
        image_writer_span = AVALON_IMG_WRITER_GRAY_SPAN;
        pixel_size = sizeof(u8) * 1;
    } else if (dev_number == MINOR_BIN) {
        printk(KERN_INFO DRIVER_NAME": Open BIN\n");
        image_writer_base = AVALON_IMG_WRITER_BINARY_BASE;
        image_writer_span = AVALON_IMG_WRITER_BINARY_SPAN;
        pixel_size = sizeof(u8) * 1;
    } else {
        printk(KERN_INFO DRIVER_NAME": Some error with the minor numbers!!\n");
        return -1;
    }

    if (is_open[dev_number] == 1) {
      printk(KERN_INFO DRIVER_NAME": This device is already open!!\n");
      return -1;
    }

    // Ioremap FPGA memory //
    // To ioremap the slave port of the image writer in the FPGA so we can access from kernel space
    address_virtual_image_writer[dev_number] =
        ioremap(HPS_FPGA_BRIDGE_BASE + image_writer_base, image_writer_span);
    if (address_virtual_image_writer[dev_number] == NULL) {
        printk(KERN_INFO DRIVER_NAME": Error doing FPGA camera ioremap\n");
        return -1;
    }

    // Calculate required memory to store an Image
    image_memory_size[dev_number] = image_width * image_height * pixel_size;

    // Allocate uncached buffers
    // The dma_alloc_coherent() function allocates non-cached physically
    // contiguous memory. Accesses to the memory by the CPU are the same
    // as a cache miss when the cache is used. The CPU does not have to
    // invalidate or flush the cache which can be time consuming.
    address_virtual_buffer0[dev_number] = dma_alloc_coherent(
        NULL,
        image_memory_size[dev_number],
        &(address_physical_buffer0[dev_number]), //address to use from image writer in fpga
        GFP_KERNEL);

    if (address_virtual_buffer0[dev_number] == NULL) {
        printk(KERN_INFO DRIVER_NAME": Allocation of non-cached buffer 0 failed\n");
        return -1;
    }

    address_virtual_buffer1[dev_number] = dma_alloc_coherent(
        NULL,
        image_memory_size[dev_number],
        &(address_physical_buffer1[dev_number]), //address to use from image writer in fpga
        GFP_KERNEL);

    if (address_virtual_buffer1[dev_number] == NULL) {
        printk(KERN_INFO DRIVER_NAME": Allocation of non-cached buffer 1 failed\n");
        return -1;
    }

    //Write the setup to the camera
    error = camera_setup(dev_number);
    if (error != 0) {
        printk(KERN_INFO DRIVER_NAME": Setup failure\n");
        return -1;
    }

    //In continuous mode start the capture of images into buff0 and buff1
    if(image_writer_mode == CONTINUOUS){
      error = camera_start_capture(dev_number);
      if (error != 0) {
          printk(KERN_INFO DRIVER_NAME": Start capture failure\n");
          return -1;
      }
    }

    is_open[dev_number] = 1;
    last_image_number[dev_number] = 0;

    return 0;
}

static ssize_t camera_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    int error;

    //Findout which device is being open using the minor numbers
    int dev_number = iminor(filep->f_path.dentry->d_inode);

    if (is_open[dev_number] == 0) {
      printk(KERN_INFO DRIVER_NAME": This device is not open!!\n");
      return -1;
    }

    error = camera_get_image(dev_number, buffer);
    if (error != 0) {
        printk(KERN_INFO DRIVER_NAME": Read failure\n");
        return -1;
    }
    return image_memory_size[dev_number];
}

static int camera_release(struct inode *inodep, struct file *filep) {

    //Findout which device is being open using the minor numbers
    int dev_number = iminor(filep->f_path.dentry->d_inode);

    if (dev_number == MINOR_RGBG) {
        printk(KERN_INFO DRIVER_NAME": Release RGBG\n");
    } else if (dev_number == MINOR_GRAY) {
        printk(KERN_INFO DRIVER_NAME": Release GRAY\n");
    } else if (dev_number == MINOR_BIN) {
        printk(KERN_INFO DRIVER_NAME": Release BIN\n");
    }

    if (is_open[dev_number] == 0) {
      printk(KERN_INFO DRIVER_NAME": Error releasing: this device is not open!!\n");
      return -1;
    }

    camera_stop_capture(dev_number);
    dma_free_coherent(NULL, image_memory_size[dev_number], address_virtual_buffer0[dev_number],
      address_physical_buffer0[dev_number]);
    dma_free_coherent(NULL, image_memory_size[dev_number], address_virtual_buffer1[dev_number],
      address_physical_buffer1[dev_number]);
    iounmap(address_virtual_image_writer[dev_number]);

    is_open[dev_number] = 0;

    return 0;
}

module_init(camera_driver_init);
module_exit(camera_driver_exit);
