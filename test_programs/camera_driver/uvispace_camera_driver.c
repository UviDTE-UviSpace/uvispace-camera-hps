#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/module.h>

#define DEVICE_NAME "uvispace_camera"
#define CLASS_NAME "uvispace_camera"

MODULE_AUTHOR("Department of Electronic Technology, University of Vigo");
MODULE_DESCRIPTION("uvispace_camera: character device driver for uvispace camera");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

static int majorNumber;
static struct class* class = NULL;
static struct device* device = NULL;

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
    return 0;
}

static ssize_t camera_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO DEVICE_NAME": Read\n");
    return 0;
}

static int camera_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO DEVICE_NAME": Release\n");
    return 0;
}

module_init(camera_driver_init);
module_exit(camera_driver_exit);
