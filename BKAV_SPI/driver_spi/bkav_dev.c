#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include "spidev.h"
#define MAX_DEV 7
//extern int FunctionA(void); 

static int open_len(struct inode *inode, struct file *file);
static int release_len(struct inode *inode, struct file *file);

static long focus_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static long zoom_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static long zoomindex_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static long iris_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static long openircut_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static long closeircut_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static long focusindex_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static const struct file_operations focus_fops = {
    .owner      = THIS_MODULE,
    .open       = open_len,
    .release    = release_len,
    .unlocked_ioctl = focus_ioctl,

};

static const struct file_operations focusIndex_fops = {
    .owner      = THIS_MODULE,
    .open       = open_len,
    .release    = release_len,
    .unlocked_ioctl = focusindex_ioctl,

};

static const struct file_operations Lenszoom_fops = {
    .owner      = THIS_MODULE,
    .open       = open_len,
    .release    = release_len,
    .unlocked_ioctl = zoom_ioctl,

};

static const struct file_operations zoomIndex_fops = {
    .owner      = THIS_MODULE,
    .open       = open_len,
    .release    = release_len,
    .unlocked_ioctl = zoomindex_ioctl,

};

static const struct file_operations moveiris_fops = {
    .owner      = THIS_MODULE,
    .open       = open_len,
    .release    = release_len,
    .unlocked_ioctl = iris_ioctl,

};

static const struct file_operations openircut_fops = {
    .owner      = THIS_MODULE,
    .open       = open_len,
    .release    = release_len,
    .unlocked_ioctl = openircut_ioctl,

};

static const struct file_operations closeircut_fops = {
    .owner      = THIS_MODULE,
    .open       = open_len,
    .release    = release_len,
    .unlocked_ioctl = closeircut_ioctl,

};

struct mychar_device_data {
    struct cdev cdev;
};

static int dev_major = 0;
static struct class *mychardev_class = NULL;
static struct mychar_device_data mychardev_data[MAX_DEV];

static int mychardev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

static int __init mychardev_init(void)
{
    int err, i;
    dev_t dev;

    err = alloc_chrdev_region(&dev, 0, MAX_DEV, "mychardev");

    dev_major = MAJOR(dev);

    mychardev_class = class_create(THIS_MODULE, "mychardev");
    mychardev_class->dev_uevent = mychardev_uevent;

    cdev_init(&mychardev_data[0].cdev, &Lenszoom_fops);
    mychardev_data[0].cdev.owner = THIS_MODULE;

    cdev_add(&mychardev_data[0].cdev, MKDEV(dev_major, 0), 1);

    device_create(mychardev_class, NULL, MKDEV(dev_major, 0), NULL, "bkav_movezoom", 0);
  	cdev_init(&mychardev_data[1].cdev, &focus_fops);
    mychardev_data[1].cdev.owner = THIS_MODULE;

    cdev_add(&mychardev_data[1].cdev, MKDEV(dev_major, 1), 1);

    device_create(mychardev_class, NULL, MKDEV(dev_major, 1), NULL, "bkav_movefocus", 1);

  	cdev_init(&mychardev_data[2].cdev, &moveiris_fops);
    mychardev_data[2].cdev.owner = THIS_MODULE;

    cdev_add(&mychardev_data[2].cdev, MKDEV(dev_major, 2), 1);

    device_create(mychardev_class, NULL, MKDEV(dev_major, 2), NULL, "bkav_moveiris", 2);

  	cdev_init(&mychardev_data[3].cdev, &closeircut_fops);
    mychardev_data[3].cdev.owner = THIS_MODULE;

    cdev_add(&mychardev_data[3].cdev, MKDEV(dev_major, 3), 1);

    device_create(mychardev_class, NULL, MKDEV(dev_major, 3), NULL, "bkav_closeircut", 3);

  	cdev_init(&mychardev_data[4].cdev, &openircut_fops);
    mychardev_data[4].cdev.owner = THIS_MODULE;

    cdev_add(&mychardev_data[4].cdev, MKDEV(dev_major, 4), 1);

    device_create(mychardev_class, NULL, MKDEV(dev_major, 4), NULL, "bkav_openircut", 4);

  	cdev_init(&mychardev_data[5].cdev, &zoomIndex_fops);
    mychardev_data[5].cdev.owner = THIS_MODULE;

    cdev_add(&mychardev_data[5].cdev, MKDEV(dev_major, 5), 1);

    device_create(mychardev_class, NULL, MKDEV(dev_major, 5), NULL, "bkav_zoomindex", 5);

    cdev_init(&mychardev_data[6].cdev, &focusIndex_fops);
    mychardev_data[6].cdev.owner = THIS_MODULE;

    cdev_add(&mychardev_data[6].cdev, MKDEV(dev_major, 6), 1);

    device_create(mychardev_class, NULL, MKDEV(dev_major, 6), NULL, "bkav_focusindex", 5);

    return 0;
}

static void __exit mychardev_exit(void)
{
    int i;

    for (i = 0; i < MAX_DEV; i++) {
        device_destroy(mychardev_class, MKDEV(dev_major, i));
    }

    class_unregister(mychardev_class);
    class_destroy(mychardev_class);

    unregister_chrdev_region(MKDEV(dev_major, 0), MINORMASK);
}


static int open_len(struct inode *inode, struct file *file)
{
    printk("BKAV Function: Device open\n");
    return 0;
}
static int release_len(struct inode *inode, struct file *file)
{
      printk("BKAV Function: Device close\n");
    return 0;
}

static long focus_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	F_MoveFocus(cmd, arg);
    printk("BKAV: Device ioctl\n");
    return 0;
}


static long iris_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	F_MoveIris(cmd);
    printk("BKAV: Device ioctl\n");
    return 0;
}


static long zoom_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    F_Lenszoom(cmd, arg);
    printk("BKAV: Device ioctl\n");
    return 0;
}


static long zoomindex_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    F_GetZoomIndex();
    printk("BKAV: Device ioctl\n");
    return 0;
}

static long focusindex_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	F_GetFocusIndex();
    printk("BKAV: Device ioctl\n");
    return 0;
}

static long openircut_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	F_OpenIRCut();
    printk("BKAV: Device ioctl\n");
    return 0;
}

static long closeircut_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	F_CloseIRCut();
    printk("BKAV: Device ioctl\n");
    return 0;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleg Kutkov <elenbert@gmail.com>");

module_init(mychardev_init);
module_exit(mychardev_exit);
