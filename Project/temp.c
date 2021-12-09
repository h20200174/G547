//Objective: Reading different parameters of LM75 sensor over I2C
//Team members: 
//Ayush Kapur (2020H1400178P)
//Nived Suresh (2020H1400174P)

#include <linux/kernel.h>      
#include <linux/module.h>      
#include <linux/fs.h>
#include <linux/uaccess.h>       
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/gpio.h>
#include "ioctl_main.h"

//Device info
#define DEVICE_NAME "lm75"
#define DEVICE_ADDR 0x48

//Register definition
#define REG_TEMP        0x00
#define REG_CONF	0x01
#define REG_THYST	0x02
#define REG_TOS	        0x03


struct class *cl;
static struct i2c_client * my_client;
struct i2c_adapter * my_adap;
static dev_t first;
static struct cdev c_dev;
static u8 val;


static u8 lm_read(struct i2c_client *client, u8 reg)	//Read function
{
	int ret;

	ret = i2c_smbus_read_byte_data(client, reg);
	if (ret < 0)
		dev_err(&client->dev,
			"Can not read register, returned %d\n", ret);

	return (u8)ret;
}

static int lm_write(struct i2c_client *client, u8 reg, u8 data)		//Write function
{
	int ret;

	ret = i2c_smbus_write_byte_data(client, reg, data);
	if (ret < 0)
		dev_err(&client->dev,"can not write register, returned %d\n", ret);

	return ret;
}

//Defining File Operations functions
static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off) {	
	
	printk(KERN_INFO "Driver: read()\n");
	return 0;
}


static int my_open(struct inode *i, struct file *f)	//Opening device
{
	  printk(KERN_INFO "Driver: open()\n");
	    return 0;
}
static int my_close(struct inode *i, struct file *f)	//Closing device file
{
	  printk(KERN_INFO "Driver: close()\n");
	    return 0;
}

static ssize_t my_write(struct file *f, const char __user *buf,
		   size_t len, loff_t *off)
{
	  printk(KERN_INFO "Driver: write()\n");
	    return len;
}

long device_ioctl(struct file *file,
                  unsigned int ioctl_num,
                  unsigned long ioctl_param)
{
    switch(ioctl_num){		//Switch cases to access different parameters

        case LM_TEMP_READ:
            val = lm_read(my_client,REG_TEMP);
	    copy_to_user((u8*)ioctl_param,&val, sizeof(val));	//Sending data to user space
 	    break;
	case LM_CONFG_READ:
            val = lm_read(my_client,REG_CONF);
	    copy_to_user((u8*)ioctl_param,&val, sizeof(val));
 	    break;
 	case LM_THYST_READ:
            val = lm_read(my_client,REG_THYST);
	    copy_to_user((u8*)ioctl_param,&val, sizeof(val));
 	    break;
 	case LM_TOS_READ:
            val = lm_read(my_client,REG_TOS);
	    copy_to_user((u8*)ioctl_param,&val, sizeof(val));
 	    break;
	case LM_SHUTDOWN:
	    val = lm_read(my_client,REG_CONF);
	    lm_write(my_client,REG_CONF,(val | 0x01));		//Setting bit 0 of configuration register
	    break;
	case LM_RESUME:
	    val = lm_read(my_client,REG_CONF);
	    lm_write(my_client,REG_CONF,(val & 0xFE));		//Resetting bit 0 of configuration register
	    break;
 	}
	//copy_to_user((u8*)ioctl_param,&val, sizeof(val));
 	return  0;
}

//Declaring and Defining File Operations structure
static struct file_operations my_dev_ops = {.read = my_read,
						.owner = THIS_MODULE,
						.open = my_open,
						.release = my_close,
						.write = my_write,
						.unlocked_ioctl = device_ioctl
						};


static int __init lm_init(void)
{
	
        printk(KERN_INFO "Welcome to LM-75!\n");
       
	// Step 1: Major & Minor number
        if (alloc_chrdev_region(&first, 0, 1, DEVICE_NAME) < 0) {
        printk(KERN_DEBUG "Can't register device\n");
        return -1;
        }

	//Step 2: Device file Creation
	if (IS_ERR(cl = class_create(THIS_MODULE, "chardrv"))){
			unregister_chrdev_region(first, 1);
			}

	if (IS_ERR(device_create(cl, NULL, first, NULL, "LM75"))) {
		class_destroy(cl);
		unregister_chrdev_region(first,1);
	}

	//Step 3: cdev Initialisation
	cdev_init(&c_dev,&my_dev_ops);
	if (cdev_add(&c_dev, first,1) < 0)
	{
		device_destroy(cl, first);
		class_destroy(cl);
		unregister_chrdev_region(first,1);
		return -1;
	}

	//Step 4: Temporary device addition
	my_adap = i2c_get_adapter(1); // Here 1 means i2c-1 bus
	if (!(my_client = i2c_new_dummy_device(my_adap, DEVICE_ADDR))){
		
		printk(KERN_INFO "Couldn't acquire i2c slave");
		unregister_chrdev(MAJOR(first), DEVICE_NAME);
		device_destroy(cl, first);
		class_destroy(cl);
		return -1;
	}
		
	return 0;
}
 

static void __exit lm_exit(void)
{
	printk(KERN_INFO "LM: Removing module");
	i2c_unregister_device(my_client);
   	cdev_del(&c_dev);
 	device_destroy(cl, first);
	class_destroy(cl);
	unregister_chrdev_region(first, 1); 
}


module_init(lm_init);
module_exit(lm_exit);

MODULE_DESCRIPTION("LM75 Driver");
MODULE_AUTHOR("Nived & Ayush");
MODULE_LICENSE("GPL");