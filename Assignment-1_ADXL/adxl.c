#include <linux/device.h>
#include <linux/module.h>
#include <linux/version.h>
#include<linux/init.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/random.h>
#include <linux/uaccess.h>

static dev_t first; //Global variable

static struct cdev c_dev_x;
static struct cdev c_dev_y;
static struct cdev c_dev_z;
static struct class *cls;
static int adxl_major;
static unsigned int i;

static int permission(struct device* dev,struct kobj_uevent_env* env)
{
  add_uevent_var(env,"DEVMODE=%#o",0666);
  return 0;
}


//////////step-4 driver callback functions//////////
static int scull_open(struct inode *i, struct file *f)
{
	printk(KERN_INFO "mychar: open()\n");
	return 0;
}
static int scull_close(struct inode *i, struct file *f)
{
	printk(KERN_INFO "mychar: close()\n");
	return 0;
}
static ssize_t scull_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "mychar: read()\n");
	get_random_bytes(&i, 2);
	i = i % 1024;
	printk(KERN_ALERT "rand=%d\n\n",i);
	if(copy_to_user(buf,&i,sizeof(i))){
	return sizeof(i); }
	return 0;
}
ssize_t scull_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "mychar: write()\n");
	return 0;
}

static struct file_operations fops =
				{
					.owner	=THIS_MODULE,
					.open	=scull_open,
					.release=scull_close,
					.read	=scull_read,
					.write	=scull_write
				};
MODULE_LICENSE("Dual BSD/GPL");

static int __init mychar_init(void)
{


//////////step-1 reserve device <major,minor>
	printk(KERN_ALERT "ADXL Registered\n\n");
	if (alloc_chrdev_region(&first, 0, 3,"BITS-Pilani")<0)
	{
		return -1;
	}
	adxl_major=MAJOR(first);
	
	
//////////step-2 creation of device file//////////
	if((cls=class_create(THIS_MODULE, "chardev"))==NULL)
	{
		unregister_chrdev_region(first, 3);
		return -1;
	}
	cls->dev_uevent = permission;
	if(device_create(cls, NULL, MKDEV(adxl_major,0), NULL, "adxl_x")==NULL)
	{
		class_destroy(cls);
		unregister_chrdev_region(first,3);
		return -1;
	}
	if(device_create(cls, NULL, MKDEV(adxl_major,1), NULL, "adxl_y")==NULL)
	{
		class_destroy(cls);
		unregister_chrdev_region(first,3);
		device_destroy(cls, MKDEV(adxl_major,0));
		return -1;
	}
	if(device_create(cls, NULL, MKDEV(adxl_major,2), NULL, "adxl_z")==NULL)
	{
		class_destroy(cls);
		unregister_chrdev_region(first,3);
		device_destroy(cls, MKDEV(adxl_major,0));
		device_destroy(cls, MKDEV(adxl_major,1));
		return -1;
	}
	
	
	//step-3 Link fops and cdev to the device node
	cdev_init(&c_dev_x, &fops);
	if(cdev_add(&c_dev_x, MKDEV(adxl_major,0), 3)==-1)
	{
		device_destroy(cls, MKDEV(adxl_major,0));
		class_destroy(cls);
		unregister_chrdev_region(first, 3);
		return -1;
	}
	cdev_init(&c_dev_y, &fops);
	if(cdev_add(&c_dev_y, MKDEV(adxl_major,1), 3)==-1)
	{
		device_destroy(cls, MKDEV(adxl_major,0));
		device_destroy(cls, MKDEV(adxl_major,1));
		class_destroy(cls);
		unregister_chrdev_region(first, 3);
		return -1;
	}
	cdev_init(&c_dev_z, &fops);
	if(cdev_add(&c_dev_x, MKDEV(adxl_major,2), 3)==-1)
	{
		device_destroy(cls, MKDEV(adxl_major,0));
		device_destroy(cls, MKDEV(adxl_major,1));
		device_destroy(cls, MKDEV(adxl_major,2));
		class_destroy(cls);
		unregister_chrdev_region(first, 3);
		return -1;
	}
	
	//printk(KERN_ALERT "<Major, Minor>: <%d, %d>\n",MAJOR(first), MINOR(first));
	return 0;
}

static void __exit mychar_exit(void)
{
	cdev_del(&c_dev_x);
	cdev_del(&c_dev_y);
	cdev_del(&c_dev_z);
	device_destroy(cls, MKDEV(adxl_major,0));
	device_destroy(cls, MKDEV(adxl_major,1));
	device_destroy(cls, MKDEV(adxl_major,2));
	class_destroy(cls);
	unregister_chrdev_region(first, 3);
	printk(KERN_ALERT "ADXL Unregistered\n\n");
}
	
module_init(mychar_init);
module_exit(mychar_exit);

MODULE_AUTHOR ( "Devansh Shah <h20180142@goa.bits-pilani.ac.in>" );
MODULE_DESCRIPTION ( "G547 Assignment 1: 3 Axis Accelerometer" );
MODULE_LICENSE ( "GPL" );
//MODULE_INFO ( Supportedchips,"PCA9685,FT232RL" );


