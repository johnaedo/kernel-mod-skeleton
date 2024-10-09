/**
 * File:	charkmod.c
 * Adapted for use in COP 4600 Operating Systems under Ubuntu or Fedora Linux
 * By:  John Aedo
 * 	
 */

#include <linux/module.h>	   // Core header for modules.
#include <linux/device.h>	   // Supports driver model.
#include <linux/kernel.h>	   // Kernel header for convenient functions.
#include <linux/fs.h>		   // File-system support.
#include <linux/uaccess.h>	   // User access copy function support.
#include <linux/version.h>	   // Allows us to check kernel version to enable variant syntax 
#define DEVICE_NAME "charkmod" // Device name.
#define MAX_SIZE 1024		   // Max buffer size.

#define DEVICE_NAME "charkmod" ///< The device will appear at /dev/skeleton using this value
#define CLASS_NAME "char"	   ///< The device class -- this is a character device driver

MODULE_LICENSE("GPL");						   ///< The license type -- this affects available functionality
MODULE_AUTHOR("John Aedo");					   ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Character device driver for COP 4600"); ///< The description -- see modinfo
MODULE_VERSION("1.0");						   ///< A version number to inform users

/**
 * Important variables that store data and keep track of relevant information.
 * 	Here you will define your buffer array and any variables needed to manipulate it
 * 	such as position markers, buffer capacity, etc.
 */


static struct class *charkmodClass = NULL;	 ///< The device-driver class struct pointer
static struct device *charkmodDevice = NULL; ///< The device-driver device struct pointer

/**
 * Prototype functions for file operations.
 */
static int open(struct inode *, struct file *);
static int close(struct inode *, struct file *);
static ssize_t read(struct file *, char *, size_t, loff_t *);
static ssize_t write(struct file *, const char *, size_t, loff_t *);

/**
 * File operations structure and the functions it points to.
 * 
 * This structure maps system-level calls to functions within the module.  
 * 	i.e., calling open("/dev/charkmod", OS_RDWR) maps the open call to the open() function
 * 	in this module.
 * 
 * 	While you can override these values to point to your own custom functions,
 * 	it is not necessary.
 */
static struct file_operations fops =
	{
		.owner = THIS_MODULE,
		.open = open,
		.release = close,
		.read = read,
		.write = write,
};

/**
 * Initializes module at installation
 */
int init_module(void)
{
	printk(KERN_INFO "charkmod: installing module.\n");

	// Allocate a major number for the device.
	major_number = register_chrdev(0, DEVICE_NAME, &fops);
	if (major_number < 0)
	{
		printk(KERN_ALERT "charkmod could not register number.\n");
		return major_number;
	}
	printk(KERN_INFO "charkmod: registered correctly with major number %d\n", major_number);

	// Register the device class using the appropriate parameters for the given kernel version.
	#if LINUX_VERSION_CODE < KERNEL_VERSION(6,4,0)
		charkmodClass = class_create(THIS_MODULE, CLASS_NAME);
	#else 
		charkmodClass = class_create(CLASS_NAME);
	#endif
	if (IS_ERR(charkmodClass))
	{ // Check for error and clean up if there is
		unregister_chrdev(major_number, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class\n");
		return PTR_ERR(charkmodClass); // Correct way to return an error on a pointer
	}
	printk(KERN_INFO "charkmod: device class registered correctly\n");

	// Register the device driver
	charkmodDevice = device_create(charkmodClass, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
	if (IS_ERR(charkmodDevice))
	{								  // Clean up if there is an error
		class_destroy(charkmodClass); // Repeated code but the alternative is goto statements
		unregister_chrdev(major_number, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create the device\n");
		return PTR_ERR(charkmodDevice);
	}
	printk(KERN_INFO "charkmod: device class created correctly\n"); // Made it! device was initialized

	// Recommendation: Initialize all data bytes of your buffer to '\0'.
	//	Do this here:

	return 0;
}

/*
 * Removes module, sends appropriate message to kernel
 */
void cleanup_module(void)
{
	printk(KERN_INFO "charkmod: removing module.\n");
	device_destroy(charkmodClass, MKDEV(major_number, 0)); // remove the device
	class_unregister(charkmodClass);					   // unregister the device class
	class_destroy(charkmodClass);						   // remove the device class
	unregister_chrdev(major_number, DEVICE_NAME);		   // unregister the major number
	printk(KERN_INFO "charkmod: Goodbye from the LKM!\n");
	unregister_chrdev(major_number, DEVICE_NAME);
	return;
}

/*
 * Opens device module, sends appropriate message to kernel
 */
static int open(struct inode *inodep, struct file *filep)
{
	printk(KERN_INFO "charkmod: device opened.\n");
	return 0;
}

/*
 * Closes device module, sends appropriate message to kernel
 */
static int close(struct inode *inodep, struct file *filep)
{
	printk(KERN_INFO "charkmod: device closed.\n");
	return 0;
}

/*
 * Reads from device, displays in userspace, and deletes the read data
 */
static ssize_t read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	/*
	Hints:
		- Use copy_to_user to transfer kernel module data to the user buffer.
		- Avoid pointer arithmetic when manipulating the various arrays.
			These will only cause you grief.
		- Use a temporary array to store the data to be copied to the user.
			Then pass that array to copy_to_user
	*/

	// Returns the number of bytes actually read back to the caller
	return len;
}

/*
 * Writes to the device
 */
static ssize_t write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	/*
	Hints:
		- Use copy_from_user to transfer user buffer data to kernel module data.
		- Avoid pointer arithmetic when manipulating the various arrays.
			These will only cause you grief.
		- Use a temporary array to store the data copied from the user.
	*/

	// Returns the number of bytes actually written
	return len;
}
