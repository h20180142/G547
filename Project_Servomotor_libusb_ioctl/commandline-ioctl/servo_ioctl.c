#include <linux/usb.h>// for usb_contro_msg
#include <linux/init.h>// macros used to markup functions e.g. __init, __exit
#include <linux/module.h>// Core header for loading LKMs into the kernel
#include <linux/kernel.h>// Contains types, macros, functions for the kernel
#include <linux/device.h>// Header to support the kernel Driver Model
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/delay.h>//mdelay function
#include "servo_ioctl.h"

static struct usb_device *device;
static struct usb_class_driver class;
unsigned int buffer[2];
uint16_t value;

static int servo_open(struct inode *i, struct file *f)
{
    printk(KERN_INFO"servo file Opened\n");   
    return 0;
}
static int servo_close(struct inode *i, struct file *f)
{
    printk(KERN_INFO"servo file Closed\n"); 
    return 0;
}

static long servo_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    int a, b;
    printk(KERN_INFO"Inside IOCTL\n");
    printk(KERN_INFO"cmd = %u arg = %lu\n",cmd,arg);
    switch(cmd)
    {
        case IOCTL_SET:           
            printk(KERN_INFO"Sending angle to USB\n");
            a = usb_control_msg(device,usb_sndctrlpipe(device,0), 0x02, USB_TYPE_VENDOR|USB_DIR_OUT, arg, 0, 0, 0, 0);
            printk(KERN_INFO"a=%d\n",a);           
            break;
        case IOCTL_STATUS:
            printk(KERN_INFO"Reading status\n");
            b = usb_control_msg(device,usb_rcvctrlpipe(device,0), 0x01, USB_TYPE_VENDOR|USB_DIR_IN, 0, 0, (unsigned int *)buffer, 2, 5000);
            printk(KERN_INFO"a=%d\n", b);
            break;
        default:
            printk(KERN_INFO"Invalid Command\n");
    }
    return 0;
}

static struct file_operations fops =
{
    .owner=THIS_MODULE,
    .open=servo_open,
    .release=servo_close,
    .unlocked_ioctl=servo_ioctl,
};


static struct usb_device_id my_id_table[]=
{
    {USB_DEVICE(0x16C0,0x05DC)},
    {}
};

MODULE_DEVICE_TABLE(usb,my_id_table);

static int my_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    int status;
    device = interface_to_usbdev(interface);
    printk(KERN_INFO "USB servo with VID=%04X PID=%04X Found\n",id->idVendor,id->idProduct);
    class.name="servo";
    class.fops=&fops;

    if((status=usb_register_dev(interface,&class))<0) /* driver registration (hot plugging) */
    {
        printk(KERN_ERR "USB servo Driver FAIL\n");
    }
    else
    {
        printk(KERN_INFO "USB servo Driver ON, Minor = %d\n",interface->minor);
    }
    return status;
}

static void my_disconnect(struct usb_interface *interface)
{
    usb_deregister_dev(interface,&class); /* dynamic driver deregistration */
    printk(KERN_INFO "USB servo Removed");
}

static struct usb_driver mydriver=
{
    .name="MyServoDriver",
    .id_table=my_id_table,
    .probe=my_probe,
    .disconnect=my_disconnect,
};

static  __init int mydriver_init(void)
{
    int ret;
    ret=usb_register(&mydriver);
    return 0;
}

static __exit void mydriver_exit(void)
{
    usb_deregister(&mydriver);
}

module_init(mydriver_init);
module_exit(mydriver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aman Bhasin");
MODULE_DESCRIPTION("Sending angle command via USB");