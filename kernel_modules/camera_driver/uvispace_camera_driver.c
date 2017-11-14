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

#define DEVICE_NAME "uvispace_camera"
#define CLASS_NAME "uvispace_camera"

#define HPS_FPGA_BRIDGE_BASE 0xC0000000

// Image types
#define RGBG   0
#define GREY   1
#define BINARY 2

// Default dimensions
#define DEFAULT_IMAGE_HEIGHT 480
#define DEFAULT_IMAGE_WIDTH  640

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
static size_t image_memory_size;

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
static int image_type = RGBG;
static int image_height = DEFAULT_IMAGE_HEIGHT;
static int image_width = DEFAULT_IMAGE_WIDTH;

static ssize_t image_type_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
  return sprintf(buf, "%d\n", image_type);
}

static ssize_t image_type_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
  sscanf(buf, "%du", &image_type);
  return count;
}
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

static struct kobj_attribute image_type_attribute = __ATTR(image_type, 0660, image_type_show, image_type_store);
static struct kobj_attribute image_height_attribute = __ATTR(image_height, 0660, image_height_show, image_height_store);
static struct kobj_attribute image_width_attribute = __ATTR(image_width, 0660, image_width_show, image_width_store);

static struct attribute *uvispace_camera_attributes[] = {
      &image_type_attribute.attr,
      &image_height_attribute.attr,
      &image_width_attribute.attr,
      NULL,
};

static struct attribute_group attribute_group = {
      .name  = "attributes",
      .attrs = uvispace_camera_attributes,    ///< The attributes array defined just above
};

static struct kobject *uvispace_camera_kobj;

static int __init camera_driver_init(void) {
    int result;

    printk(KERN_INFO DEVICE_NAME": Init\n");
    // Dynamically allocate a major number for the device
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        printk(KERN_ALERT DEVICE_NAME": Failed to register a major number\n");
        return 1;
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
    // Export sysfs variables
    // kernel_kobj points to /sys/kernel
    uvispace_camera_kobj = kobject_create_and_add("uvispace_camera", kernel_kobj->parent);
    if (!uvispace_camera_kobj) {
       printk(KERN_INFO DEVICE_NAME": Failed to create kobject mapping\n");
       return 1;
    }
    // add the attributes to /sys/uvispace_camera/attributes
    result = sysfs_create_group(uvispace_camera_kobj, &attribute_group);
    if (result) {
       printk(KERN_INFO DEVICE_NAME": Failed to create sysfs group\n");
       return result;
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
    int image_writer_base;
    int image_writer_span;
    int pixel_size;

    printk(KERN_INFO DEVICE_NAME": Open\n");

    // Establish image_writer and pixel_size based on image_type
    if (image_type == RGBG) {
        image_writer_base = AVALON_IMG_WRITER_RGBGRAY_BASE;
        image_writer_span = AVALON_IMG_WRITER_RGBGRAY_SPAN;
        pixel_size = sizeof(u8) * 4;
    } else if (image_type == GREY) {
        image_writer_base = AVALON_IMG_WRITER_GRAY_BASE;
        image_writer_span = AVALON_IMG_WRITER_GRAY_SPAN;
        pixel_size = sizeof(u8) * 1;
    } else if (image_type == BINARY) {
        image_writer_base = AVALON_IMG_WRITER_BINARY_BASE;
        image_writer_span = AVALON_IMG_WRITER_BINARY_SPAN;
        pixel_size = sizeof(u8) * 1;
    } else {
        printk(KERN_INFO DEVICE_NAME": Not a valid image type\n");
        return -1;
    }

    // Ioremap FPGA memory //
    // To ioremap the memory in the FPGA so we can access from kernel space
    address_virtual_camera = ioremap(HPS_FPGA_BRIDGE_BASE + image_writer_base, image_writer_span);
    if (address_virtual_camera == NULL) {
        printk(KERN_INFO DEVICE_NAME": Error doing FPGA camera ioremap\n");
        return -1;
    }

    // Calculate required memory to store an Image
    image_memory_size = image_width * image_height * pixel_size;

    // Allocate uncached buffer
    // The dma_alloc_coherent() function allocates non-cached physically
    // contiguous memory. Accesses to the memory by the CPU are the same
    // as a cache miss when the cache is used. The CPU does not have to
    // invalidate or flush the cache which can be time consuming.
    address_virtual_buffer = dma_alloc_coherent(
        NULL,
        image_memory_size,
        &address_physical_buffer, //address to use from DMAC
        GFP_KERNEL);

    if (address_virtual_buffer == NULL) {
        printk(KERN_INFO DEVICE_NAME": Allocation of non-cached buffer failed\n");
        return -1;
    }

    return 0;
}

int camera_capture_image(char* user_read_buffer) {
    int counter;
    int error_count;
    int i;
    // Save physical addresses into the avalon_camera
    iowrite32(address_physical_buffer, address_virtual_camera + CAPTURE_BUFF0);
    iowrite32(address_physical_buffer, address_virtual_camera + CAPTURE_BUFF1);
    iowrite32(0, address_virtual_camera + CAPTURE_BUFFER_SELECT);

    // Indicate the image size to the capture_image component
    // TODO update image size
    iowrite32(image_width, address_virtual_camera + CAPTURE_WIDTH);
    iowrite32(image_height, address_virtual_camera + CAPTURE_HEIGHT);

    // Set up downsampling as 1 to get the whole image
    iowrite32(1, address_virtual_camera + CAPTURE_DOWNSAMPLING);

    // Wait until Standby signal is 1. Its the way to ensure that the component
    // is not in reset or acquiring a signal.
    counter = 10000000;
    while((!(ioread32(address_virtual_camera + CAPTURE_STANDBY))) && (counter>0)) {
        // Ugly way avoid software to get stuck
        counter--;
    }
    if (counter == 0) {
        printk(KERN_INFO DEVICE_NAME": Camera no reply\n");
        return ERROR_CAMERA_NO_REPLY;
    }

    // Start the capture (generate a pos flank in start_capture signal)
    iowrite32(1, address_virtual_camera + START_CAPTURE);
    iowrite32(0, address_virtual_camera + START_CAPTURE);

    // Wait for the image to be acquired
    while (!ioread32(address_virtual_camera + CAPTURE_STANDBY)) {}

    for (i = 0; i < 20; i += 1)
    {
      printk(KERN_INFO DEVICE_NAME": buff[%d] = %d", i, ((int*)address_virtual_buffer)[i]);
    }

    // Copy the image from buffer camera buffer to user buffer
    error_count = copy_to_user(user_read_buffer, address_virtual_buffer, image_memory_size);

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
    return image_memory_size;
}

static int camera_release(struct inode *inodep, struct file *filep) {
    dma_free_coherent(NULL, image_memory_size, address_virtual_buffer, address_physical_buffer);
    iounmap(address_virtual_camera);
    printk(KERN_INFO DEVICE_NAME": Release\n");
    return 0;
}

module_init(camera_driver_init);
module_exit(camera_driver_exit);
