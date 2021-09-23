#include <linux/kernel.h>       /* We're doing kernel work */
#include <linux/module.h>
#include<linux/version.h>       /* Specifically, a module */
#include <linux/fs.h>
#include <linux/uaccess.h>        /* for get_user and put_user */
#include <linux/time.h>
#include <linux/random.h>
#include <linux/errno.h>
#include<linux/init.h>
#include<linux/types.h>
#include<linux/kdev_t.h>
#include<linux/device.h>
#include<linux/cdev.h>

/* Header file containing IOCTL declaration */
#include "imu_header.h"

#define SUCCESS 0
#define DEVICE_NAME "imu_char"

static dev_t first;
static struct cdev c_dev;
static struct class *cls;

/*
 * Is the device open right now? Used to prevent
 * concurent access into the same device
 */
static int Device_Open = 0;

static int device_open(struct inode *inode, struct file *file)
{
#ifdef DEBUG
        printk(KERN_INFO "device_open(%p)\n", file);
#endif
    /*
     * We don't want to talk to two processes at the same time
     */
    if (Device_Open)
        return -EBUSY;

    Device_Open++;
    try_module_get(THIS_MODULE);
    return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
#ifdef DEBUG
    printk(KERN_INFO "device_release(%p,%p)\n", inode, file);
#endif

    /*
     * We're now ready for our next caller
     */
    Device_Open--;

    module_put(THIS_MODULE);
    return SUCCESS;
}

static ssize_t device_read(struct file *file,   /* see include/linux/fs.h   */
                           char __user * buffer,        /* buffer to be
                                                         * filled with data */
                           size_t length,       /* length of the buffer     */
                           loff_t * offset)
{

    uint16_t val;
	char *new;
	char random[8];

    /* Function to generate random value */
	get_random_bytes(&val, 2);		
	
	val = val>>1;
	sprintf(random,"%d\n",val);     //Function to write int value to string
	new = random;

    /* Function to copy data from kernel space to user space */ 
	copy_to_user(buffer, new, sizeof(new));
         
#ifdef DEBUG
    printk(KERN_INFO "device_read(%p,%p,%d)\n", file, buffer, length);
#endif

    return (sizeof(buffer));
}

static ssize_t device_write(struct file *file,
             const char __user * buffer, size_t length, loff_t * offset)
{

	#ifdef DEBUG
	printk(KERN_INFO "device_write(%p,%s,%d)", file, buffer, length);
	#endif
	return SUCCESS;
}

/*
----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------
*/

/* IOCTL Callback function definition */  

long device_ioctl(struct file *file,             /* ditto */
                  unsigned int ioctl_num,        /* number and param for ioctl */
                  unsigned long ioctl_param)
{
    int i;
    switch(ioctl_num){

        case IOCTL_GET_ACCx:
            i = device_read(file, (char *)ioctl_param, 99, 0);

            /*
            * Put a zero at the end of the buffer, so it will be
            * properly terminated
            */
            put_user('\0', (char *)ioctl_param + i);
 	    break;

        case IOCTL_GET_ACCy:
	
            i = device_read(file, (char *)ioctl_param, 99, 0);

            /*
            * Put a zero at the end of the buffer, so it will be
            * properly terminated
            */
            put_user('\0', (char *)ioctl_param + i);
	    break;

	case IOCTL_GET_ACCz:
	
            i = device_read(file, (char *)ioctl_param, 99, 0);
	    
            /*
            * Put a zero at the end of the buffer, so it will be
            * properly terminated
            */
            put_user('\0', (char *)ioctl_param + i);
	    break;

	case IOCTL_GET_GYROx:
	
            i = device_read(file, (char *)ioctl_param, 99, 0);

            /*
            * Put a zero at the end of the buffer, so it will be
            * properly terminated
            */
            put_user('\0', (char *)ioctl_param + i);
	    break;

	case IOCTL_GET_GYROy:
	
            i = device_read(file, (char *)ioctl_param, 99, 0);

            /*
            * Put a zero at the end of the buffer, so it will be
            * properly terminated
            */
            put_user('\0', (char *)ioctl_param + i);
	    break;

	case IOCTL_GET_GYROz:
	
            i = device_read(file, (char *)ioctl_param, 99, 0);

            /*
            * Put a zero at the end of the buffer, so it will be
            * properly terminated
            */
            put_user('\0', (char *)ioctl_param + i);
	    break;

	case IOCTL_GET_MAGx:
	
            i = device_read(file, (char *)ioctl_param, 99, 0);

            /*
            * Put a zero at the end of the buffer, so it will be
            * properly terminated
            */
            put_user('\0', (char *)ioctl_param + i);
	    break;
	
	case IOCTL_GET_MAGy:
	
            i = device_read(file, (char *)ioctl_param, 99, 0);

            /*
            * Put a zero at the end of the buffer, so it will be
            * properly terminated
            */
            put_user('\0', (char *)ioctl_param + i);
	    break;

	case IOCTL_GET_MAGz:
	
            i = device_read(file, (char *)ioctl_param, 99, 0);

            /*
            * Put a zero at the end of the buffer, so it will be
            * properly terminated
            */
            put_user('\0', (char *)ioctl_param + i);
	    break;

	case IOCTL_GET_PRESS:
	
            i = device_read(file, (char *)ioctl_param, 99, 0);
	    
            /*
            * Put a zero at the end of the buffer, so it will be
            * properly terminated
            */
            put_user('\0', (char *)ioctl_param + i);
	    break;
    }

    return SUCCESS; 
}

struct file_operations fops = {
        .read = device_read,
        .write = device_write,
        .unlocked_ioctl = device_ioctl,
        .open = device_open,
        .release = device_release,      /* a.k.a. close */
};

/* Kernel Initialisation function */
static int __init mychar_init(void)
{

	printk(KERN_INFO "Mychar driver registered \n");
	if (alloc_chrdev_region(&first,0,1,"Bits_Pilani") < 0)
	{
		return -1;
	}
	
	if((cls = class_create(THIS_MODULE,"imu_dev")) == NULL)
	{
		unregister_chrdev_region(first,1);
		return -1;
	}
	if(device_create(cls,NULL,first,NULL,DEVICE_NAME) == NULL)
	{
		class_destroy(cls);
		unregister_chrdev_region(first,1);
		return -1;
	}

	cdev_init(&c_dev,&fops);
	if(cdev_add(&c_dev,first,1) == -1)
	{
		device_destroy(cls,first);
		class_destroy(cls);
		unregister_chrdev_region(first,1);
		return -1;
	}
	
	printk(KERN_INFO " <Major,Minor> : <%d, %d>\n",MAJOR(first),MINOR(first));
	return 0;
}


/* Kernel Exit function */
static void __exit mychar_exit(void)
{
	cdev_del(&c_dev);
	unregister_chrdev_region(first,1);
	device_destroy(cls,first);
	class_destroy(cls);
	printk(KERN_INFO "Mychar driver unregistered \n");
	
}

module_init(mychar_init);
module_exit(mychar_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("DRIVER FOR IMU");
MODULE_AUTHOR("NIVED SURESH <h20200174@pilani.bits-pilani.ac.in>");
MODULE_INFO(ChipSupport, "MPU9255, BMP280");